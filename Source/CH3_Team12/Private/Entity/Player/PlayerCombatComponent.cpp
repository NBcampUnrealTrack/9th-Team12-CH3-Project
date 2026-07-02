#include "Entity/Player/PlayerCombatComponent.h"
#include "GameplayTags/CombatGameplayTags.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/StateTagComponent.h"

UPlayerCombatComponent::UPlayerCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	OwnerCharacter = Cast<APlayerCharacterBase>(GetOwner());
}


void UPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	
}

void UPlayerCombatComponent::Attack()
{
	if (IsBusy())
	{
		if (bComboWindow)
		{
			bComboBuffered = true;
		}

		return;
	}

	StartAttack(EAttackType::Light);
}

void UPlayerCombatComponent::HeavyAttack()
{
	if (!CanAttack())
	{
		return;
	}

	StartAttack(EAttackType::Heavy);
}

void UPlayerCombatComponent::StartAttack(EAttackType AttackType)
{
	ComboIndex = 0;
	bComboBuffered = false;
	
	OwnerCharacter->GetStateTagComponent()->AddStateTag(CombatTags::State_Combat_Attacking);
	
	switch (AttackType)
	{
	case EAttackType::Light:
		OwnerCharacter->PlayAnimMontage(LightAttackMontage);
		break;
	case EAttackType::Heavy:
		OwnerCharacter->PlayAnimMontage(HeavyAttackMontage);
		break;
	}
}

void UPlayerCombatComponent::EndAttack()
{
	ComboIndex = 0;
	bComboBuffered = false;
	bComboWindow = false;

	DisableWeaponCollision();
	
	OwnerCharacter->GetStateTagComponent()->RemoveStateTag(CombatTags::State_Combat_Attacking);
	
}

void UPlayerCombatComponent::ContinueCombo()
{
	bComboBuffered = false;

	ComboIndex++;

	if (ComboIndex >= MaxComboCount)
	{
		return;
	}
	
	UAnimInstance* AnimInstance =
		OwnerCharacter->GetMesh()->GetAnimInstance();

	if (!AnimInstance)
	{
		return;
	}
	
	AnimInstance->Montage_JumpToSection(
		ComboSectionNames[ComboIndex],
		LightAttackMontage);
}

void UPlayerCombatComponent::StartComboWindow()
{
	bComboWindow = true;
}

void UPlayerCombatComponent::EndComboWindow()
{
	bComboWindow = false;

	if (bComboBuffered)
	{
		ContinueCombo();
	}
}

void UPlayerCombatComponent::EnableWeaponCollision()
{
	bWeaponCollision = true;

	// 무기 Collision ON
}

void UPlayerCombatComponent::DisableWeaponCollision()
{
	bWeaponCollision = false;

	// 무기 Collision OFF
}

void UPlayerCombatComponent::EnableInvincible()
{
	bInvincible = true;

	// Status.Invincible 추가
}

void UPlayerCombatComponent::DisableInvincible()
{
	bInvincible = false;

	// Status.Invincible 제거
}

void UPlayerCombatComponent::Dodge()
{
	if (!CanDodge())
	{
		return;
	}

	OwnerCharacter->GetStateTagComponent()->AddStateTag(CombatTags::State_Combat_Dodging);

	OwnerCharacter->PlayAnimMontage(DodgeMontage);
}

void UPlayerCombatComponent::EndDodge()
{
	DisableInvincible();

	if (OwnerCharacter->GetStateTagComponent()->HasStateTag(CombatTags::State_Combat_Dodging))
	{
		OwnerCharacter->GetStateTagComponent()->RemoveStateTag(CombatTags::State_Combat_Dodging);
	}
}

bool UPlayerCombatComponent::CanAttack() const
{
	return !IsBusy();
}

bool UPlayerCombatComponent::CanDodge() const
{
	return !IsBusy();
}

bool UPlayerCombatComponent::IsBusy() const
{
	if (!OwnerCharacter) return true;
	
	UStateTagComponent* StateComp = OwnerCharacter->GetStateTagComponent();
	
	if (!StateComp) return true;
	
	FGameplayTagContainer BusyTags;
	BusyTags.AddTag(CombatTags::State_Combat_Attacking);
	BusyTags.AddTag(CombatTags::State_Combat_Dodging);
	BusyTags.AddTag(CombatTags::State_Combat_Parry);

	return StateComp->HasAnyStateTags(BusyTags);
}