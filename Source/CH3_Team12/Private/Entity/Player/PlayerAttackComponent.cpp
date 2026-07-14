#include "Entity/Player/PlayerAttackComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/StateTagComponent.h"
#include "Entity/Player/PlayerLocomotionComponent.h"
#include "Entity/Player/PlayerAttributeComponent.h"
#include "Entity/Player/PlayerEquipmentComponent.h"
#include "Entity/Player/PlayerWeaponComponent.h"
#include "Entity/Enemy/EnemyCharacterBase.h"
#include "Entity/Enemy/Component/EnemyAttributeComponent.h"
#include "Entity/Enemy/Component/EnemyDefenseComponent.h"
#include "Framework/DataAsset/EnemyExecutionDataAsset.h"
#include "GameplayTags/CombatGameplayTags.h"
#include "Framework/DataAsset/PlayerAttackDataAsset.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Entity/Weapon/WeaponBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Entity/Item/ItemInstance.h"
#include "Framework/DataAsset/WeaponDataAsset.h"

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
	bool bInterrupted)
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

	if (!StateComponent->HasStateTagExact(
		CombatTags::State_Combat_Attacking))
	{
		return;
	}

	// 공격 상태는 유지.
	// 단, 이 시점부터 Dodge Cancel 가능.
	bDodgeCancelWindowOpen = true;
	bMoveCancelWindowOpen = true;

	StateComponent->RemoveStateTag(
		CombatTags::State_Movement_Locked
	);

	if (UPlayerLocomotionComponent* LocomotionComponent =
		OwnerCharacter->GetLocomotionComponent())
	{
		LocomotionComponent->RefreshMovementSettings();
		
		if (LocomotionComponent->TryConsumeBufferedDodge())
		{
			return;
		}
	}
}

const UPlayerAttackDataAsset* UPlayerAttackComponent::GetAttackData() const
{
	if (!EquipmentComponent)
	{
		return nullptr;
	}

	const UWeaponDataAsset* WeaponData = 
		EquipmentComponent->GetEquippedWeaponData();

	if (!WeaponData)
	{
		return nullptr;
	}
	
	return WeaponData->AttackData;
}

const FAttackDefinition* UPlayerAttackComponent::GetAttackDataByType(EAttackType AttackType) const
{
	const UPlayerAttackDataAsset* Data = GetAttackData();

	if (!Data)
	{
		return nullptr;
	}

	switch (AttackType)
	{
	case EAttackType::Light:
		return &Data->LightAttack;
	case EAttackType::Heavy:
		return &Data->HeavyAttack;
	case EAttackType::Jump:
		return &Data->JumpAttack;
	case EAttackType::Dodge:
		return &Data->DodgeAttack;
	case EAttackType::Execution:
		return &Data->ExecutionAttack;
	default:
		return nullptr;
	}
}

void UPlayerAttackComponent::Attack(const FInputActionValue& Value)
{
	if (CanExecution())
	{
		StartExecution();
		return;
	}
	
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

	if (!CanStartAttack())
	{
		return;
	}

	const FAttackDefinition* AttackData = nullptr;
	
	if (OwnerCharacter->GetCharacterMovement()->IsFalling())
	{
		AttackData = GetAttackDataByType(EAttackType::Jump);
	}
	else if (bCanDodgeAttack)
	{
		AttackData = GetAttackDataByType(EAttackType::Dodge);
	}
	else
	{
		AttackData = GetAttackDataByType(EAttackType::Light);
	}
	
	if (AttackData)
	{
		StartAttack(AttackData);
	}
}

void UPlayerAttackComponent::HeavyAttack(const FInputActionValue& Value)
{
	if (!CanStartAttack())
	{
		return;
	}

	StartAttack(
		GetAttackDataByType(EAttackType::Heavy));
}


void UPlayerAttackComponent::CancelAttackInternal()
{
	bDodgeCancelWindowOpen = false;
	bMoveCancelWindowOpen = false;
	bComboWindow = false;
	bComboBuffered = false;
	ComboIndex = 0;

	if (WeaponComponent)
	{
		WeaponComponent->EndWeaponHitCheck();
	}

	UAnimMontage* MontageToStop = CurrentAttackMontage;
	CurrentAttackMontage = nullptr;

	StateComponent->RemoveStateTag(CombatTags::State_Combat_Attacking);
	StateComponent->RemoveStateTag(CombatTags::State_Movement_Locked);

	if (MontageToStop)
	{
		if (USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh())
		{
			if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
			{
				AnimInstance->Montage_Stop(0.08f, MontageToStop);
			}
		}
	}
}

void UPlayerAttackComponent::CancelAttackForDodge()
{
	CancelAttackInternal();
}

void UPlayerAttackComponent::CancelAttackForMovement()
{
	CancelAttackInternal();
}

void UPlayerAttackComponent::CancelAttackForHit()
{
	CancelAttackInternal();
}

void UPlayerAttackComponent::CancelAttackForDeath()
{
	CancelAttackInternal();
}

void UPlayerAttackComponent::CancelAttackForPostureBreak()
{
	CancelAttackInternal();
}

bool UPlayerAttackComponent::CanStartAttack() const
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
	BlockTags.AddTag(CombatTags::State_Action_UsingItem);

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

bool UPlayerAttackComponent::CanContinueCombo() const
{
	if (!CurrentAttackData)
	{
		return false;
	}
	
	return CurrentAttackData->Steps.IsValidIndex(ComboIndex + 1); 
}

void UPlayerAttackComponent::StartAttack(
	const FAttackDefinition* AttackInfo)
{
	if (!OwnerCharacter ||
		!StateComponent ||
		!AttackInfo		||
		!AttackInfo->Montage ||
		AttackInfo->Steps.IsEmpty())
	{
		return;
	}

	UAnimInstance* AnimInstance =
		OwnerCharacter->GetMesh()->GetAnimInstance();

	if (!AnimInstance)
	{
		return;
	}

	CurrentAttackData = AttackInfo;
	CurrentAttackMontage = AttackInfo->Montage;
	ComboIndex = 0;
	CurrentStep = &CurrentAttackData->Steps[ComboIndex];
	
	bComboWindow = false;
	bComboBuffered = false;

	StateComponent->AddStateTag(
		CombatTags::State_Combat_Attacking);

	StateComponent->AddStateTag(
		CombatTags::State_Movement_Locked);
	
	const float Duration =
		AnimInstance->Montage_Play(
			AttackInfo->Montage,
			CurrentStep->PlayRate);

	if (Duration <= 0.f)
	{
		EndAttack();
		return;
	}
	AnimInstance->Montage_JumpToSection(
			CurrentStep->SectionName,
			AttackInfo->Montage);

	FOnMontageEnded Delegate;

	Delegate.BindUObject(
		this,
		&UPlayerAttackComponent::OnAttackMontageEnded);

	AnimInstance->Montage_SetEndDelegate(
		Delegate,
		AttackInfo->Montage);
}

void UPlayerAttackComponent::EndAttack()
{
	WeaponComponent->EndWeaponHitCheck();

	CurrentAttackMontage = nullptr;
	CurrentAttackData = nullptr;
	CurrentStep = nullptr;
	
	ComboIndex = 0;
	
	bComboWindow = false;
	bComboBuffered = false;
	bDodgeCancelWindowOpen = false;
	bMoveCancelWindowOpen = false;
	bCanDodgeAttack = false;
	
	if (StateComponent)
	{
		StateComponent->RemoveStateTag(
			CombatTags::State_Combat_Attacking);
		StateComponent->RemoveStateTag(
			CombatTags::State_Movement_Locked);
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

void UPlayerAttackComponent::CanDodgeAttack()
{
	bCanDodgeAttack = true;
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
	if (!OwnerCharacter ||
		!StateComponent ||
		!CurrentAttackData)
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

	CurrentStep = &CurrentAttackData->Steps[ComboIndex];
	
	bComboBuffered = false;
	bComboWindow = false;
	
	UAnimInstance* AnimInstance =
		OwnerCharacter->GetMesh()
			? OwnerCharacter->GetMesh()->GetAnimInstance()
			: nullptr;

	if (!AnimInstance)
	{
		return;
	}

	StateComponent->AddStateTag(
		CombatTags::State_Movement_Locked);

	AnimInstance->Montage_SetPlayRate(
		CurrentAttackMontage,
		CurrentStep->PlayRate);

	AnimInstance->Montage_JumpToSection(
		CurrentStep->SectionName,
		CurrentAttackMontage);
}

void UPlayerAttackComponent::StartExecution()
{
	AEnemyCharacterBase* Enemy = ExecutableEnemy.Get();

	if (!Enemy)
	{
		return;
	}

	const UEnemyExecutionDataAsset* Data =
		Enemy->GetExecutionData();

	if (!Data)
	{
		return;
	}

	// 플레이어 위치 이동
	const FVector TargetLocation =
		Enemy->GetMesh()->GetSocketLocation(
			Data->PlayerExecutionSocket);

	OwnerCharacter->SetActorLocation(TargetLocation);

	// 플레이어 몽타주
	OwnerCharacter->PlayAnimMontage(
		Data->PlayerExecutionMontage);

	// 적 몽타주
	Enemy->PlayAnimMontage(
		Data->EnemyExecutionMontage);
}

void UPlayerAttackComponent::RegisterEnemy(AEnemyCharacterBase* Enemy)
{
	if (!Enemy)
	{
		return;
	}

	UEnemyAttributeComponent* Attribute =
		Enemy->GetEnemyAttributeComponent();

	if (!Attribute)
	{
		return;
	}

	Attribute->OnEnemyPostureBroken.AddDynamic(
		this,
		&ThisClass::OnEnemyPostureBroken);

	Attribute->OnEnemyPostureRecovered.AddDynamic(
		this,
		&ThisClass::OnEnemyPostureRecovered);

	Attribute->OnEnemyDeath.AddDynamic(
		this,
		&ThisClass::OnEnemyDeath);
}

void UPlayerAttackComponent::OnEnemyPostureBroken()
{
	AEnemyCharacterBase* Enemy =
		Cast<AEnemyCharacterBase>(GetCurrentLockOnTarget());

	if (!Enemy)
	{
		return;
	}

	ExecutableEnemy = Enemy;
}

void UPlayerAttackComponent::OnEnemyPostureRecovered()
{
	ExecutableEnemy = nullptr;
}

void UPlayerAttackComponent::OnEnemyDeath()
{
	ExecutableEnemy = nullptr;
}

bool UPlayerAttackComponent::CanExecution() const
{
	if (!ExecutableEnemy.IsValid())
	{
		return false;
	}

	const UEnemyExecutionDataAsset* Data =
		ExecutableEnemy->GetExecutionData();

	if (!Data)
	{
		return false;
	}

	return true;
}

const FAttackHitData*
UPlayerAttackComponent::GetCurrentHit(int32 HitIndex) const
{
	if (!CurrentStep)
		return nullptr;

	if (!CurrentStep->Hits.IsValidIndex(HitIndex))
		return nullptr;

	return &CurrentStep->Hits[HitIndex];
}
