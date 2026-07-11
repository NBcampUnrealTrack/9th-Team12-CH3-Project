#include "Entity/Player/PlayerAttackComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/StateTagComponent.h"
#include "Entity/Player/PlayerLocomotionComponent.h"
#include "Entity/Player/PlayerAttributeComponent.h"
#include "Entity/Player/PlayerEquipmentComponent.h"
#include "Entity/Player/PlayerWeaponComponent.h"
#include "GameplayTags/CombatGameplayTags.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Entity/Weapon/WeaponBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

UPlayerAttackComponent::UPlayerAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<APlayerCharacterBase>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCombatComponent : OwnerCharacter is nullptr"));
		return;
	}

	StateComponent = OwnerCharacter->GetStateTagComponent();
	if (!StateComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCombatComponent : StateComponent is nullptr"));
		return;
	}
	
	AttributeComponent = OwnerCharacter->GetAttributeComponent();
	if (!AttributeComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCombatComponent : AttributeComponent is nullptr"));
	}
	
	EquipmentComponent = OwnerCharacter->GetEquipmentComponent();
	if (!EquipmentComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCombatComponent : EquipmentComponent is nullptr"));
	}
	
	WeaponComponent = OwnerCharacter->GetWeaponComponent();
	if (!WeaponComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCombatComponent : WeaponComponent is nullptr"));
	}
}

void UPlayerAttackComponent::OnAttackMontageEnded(
	UAnimMontage* Montage,
	bool bInterrupted
)
{
	if (Montage != CurrentAttackMontage)
	{
		return;
	}

	EndAttack();
}

void UPlayerAttackComponent::OpenAttackRecovery()
{
	if (!OwnerCharacter || !StateComponent)
	{
		return;
	}

	if (!StateComponent->HasStateTagExact(CombatTags::State_Combat_Attacking))
	{
		return;
	}

	// 공격 상태는 유지.
	// 이동 잠금만 해제.
	StateComponent->RemoveStateTag(
		CombatTags::State_Movement_Locked
	);

	if (UPlayerLocomotionComponent* LocomotionComponent =
		OwnerCharacter->GetLocomotionComponent())
	{
		LocomotionComponent->RefreshMovementSettings();
	}
}

void UPlayerAttackComponent::Attack(const FInputActionValue& Value)
{
	if (IsAttacking())
	{
		if (!CanContinueCombo())
		{
			return;
		}

		if (bComboWindow)
		{
			ContinueCombo();
		}
		else
		{
			bComboBuffered = true;
		}

		return;
	}

	if (!CanAttack())
	{
		return;
	}

	StartAttack(EAttackType::Light);
}

void UPlayerAttackComponent::HeavyAttack(const FInputActionValue& Value)
{
	if (!CanAttack())
	{
		return;
	}

	StartAttack(EAttackType::Heavy);
}

bool UPlayerAttackComponent::CanAttack() const
{
	if (!OwnerCharacter || !StateComponent)
	{
		return false;
	}

	if (!EquipmentComponent->GetEquippedWeapon())
	{
		return false;
	}

	if (!StateComponent->HasStateTagExact(CombatTags::State_Combat_Armed))
	{
		return false;
	}
	
	FGameplayTagContainer BlockTags;
	BlockTags.AddTag(CombatTags::State_Combat_Dodging);
	BlockTags.AddTag(CombatTags::State_Combat_Guarding);
	BlockTags.AddTag(CombatTags::State_Combat_Parry);
	BlockTags.AddTag(CombatTags::State_Movement_Locked);
	BlockTags.AddTag(CombatTags::State_Hit_PostureBroken);
	BlockTags.AddTag(CombatTags::State_Hit_Dead);
	BlockTags.AddTag(CombatTags::State_Hit_Reacting);

	return !StateComponent->HasAnyStateTags(BlockTags);
}

bool UPlayerAttackComponent::IsAttacking() const
{
	if (!StateComponent)
	{
		return false;
	}

	return StateComponent->HasStateTagExact(
		CombatTags::State_Combat_Attacking
	);
}

bool UPlayerAttackComponent::IsBusy() const
{
	if (!StateComponent)
	{
		return true;
	}

	FGameplayTagContainer BusyTags;
	BusyTags.AddTag(CombatTags::State_Combat_Attacking);
	BusyTags.AddTag(CombatTags::State_Combat_Dodging);
	BusyTags.AddTag(CombatTags::State_Combat_Guarding);
	BusyTags.AddTag(CombatTags::State_Hit_PostureBroken);
	BusyTags.AddTag(CombatTags::State_Hit_Dead);

	return StateComponent->HasAnyStateTags(BusyTags);
}

bool UPlayerAttackComponent::CanContinueCombo() const
{
	const int32 NextComboIndex = ComboIndex + 1;
	return AttackData->ComboSectionNames.IsValidIndex(NextComboIndex);
}

void UPlayerAttackComponent::StartAttack(EAttackType AttackType)
{
	if (!OwnerCharacter || !StateComponent)
	{
		return;
	}

	UAnimMontage* AttackMontage = nullptr;

	switch (AttackType)
	{
	case EAttackType::Light:
		AttackMontage = AttackData->LightAttackMontage;
		break;

	case EAttackType::Heavy:
		AttackMontage = AttackData->HeavyAttackMontage;
		break;
	}

	if (!AttackMontage)
	{
		return;
	}

	UAnimInstance* AnimInstance =
		OwnerCharacter->GetMesh()
			? OwnerCharacter->GetMesh()->GetAnimInstance()
			: nullptr;

	if (!AnimInstance)
	{
		return;
	}

	CurrentAttackType = AttackType;
	CurrentAttackMontage = AttackMontage;

	ComboIndex = 0;
	bComboWindow = false;
	bComboBuffered = false;

	StateComponent->AddStateTag(CombatTags::State_Combat_Attacking);
	StateComponent->AddStateTag(CombatTags::State_Movement_Locked);

	const float Duration =
		AnimInstance->Montage_Play(AttackMontage, AttackData->AttackPlayRate);

	if (Duration <= 0.0f)
	{
		EndAttack();
		return;
	}

	AnimInstance->Montage_JumpToSection(
		AttackData->ComboSectionNames[ComboIndex],
		AttackMontage
	);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(
		this,
		&UPlayerAttackComponent::OnAttackMontageEnded
	);

	AnimInstance->Montage_SetEndDelegate(
		EndDelegate,
		AttackMontage
	);
}

void UPlayerAttackComponent::EndAttack()
{
	WeaponComponent->EndWeaponHitCheck();

	CurrentAttackMontage = nullptr;

	ComboIndex = 0;
	bComboWindow = false;
	bComboBuffered = false;

	if (StateComponent)
	{
		StateComponent->RemoveStateTag(CombatTags::State_Combat_Attacking);
		StateComponent->RemoveStateTag(CombatTags::State_Movement_Locked);
	}

	if (OwnerCharacter)
	{
		if (UPlayerLocomotionComponent* LocomotionComponent =
			OwnerCharacter->GetLocomotionComponent())
		{
			LocomotionComponent->RefreshMovementSettings();
		}
	}
}

void UPlayerAttackComponent::OpenComboWindow()
{
	if (!CanContinueCombo())
	{
		return;
	}

	bComboWindow = true;

	if (bComboBuffered)
	{
		bComboBuffered = false;
		ContinueCombo();
	}
}

void UPlayerAttackComponent::ContinueCombo()
{
	if (!OwnerCharacter || !StateComponent)
	{
		return;
	}

	if (!CanContinueCombo())
	{
		bComboBuffered = false;
		bComboWindow = false;
		return;
	}

	++ComboIndex;

	bComboBuffered = false;
	bComboWindow = false;

	UAnimMontage* CurrentMontage =
		CurrentAttackType == EAttackType::Light
			? AttackData->LightAttackMontage
			: AttackData->HeavyAttackMontage;

	if (!CurrentMontage)
	{
		return;
	}

	UAnimInstance* AnimInstance =
		OwnerCharacter->GetMesh()
			? OwnerCharacter->GetMesh()->GetAnimInstance()
			: nullptr;

	if (!AnimInstance)
	{
		return;
	}

	StateComponent->AddStateTag(CombatTags::State_Movement_Locked);

	AnimInstance->Montage_JumpToSection(
		AttackData->ComboSectionNames[ComboIndex],
		CurrentMontage
	);
}