#include "Entity/Player/PlayerCombatComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/StateTagComponent.h"
#include "GameplayTags/CombatGameplayTags.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Entity/Weapon/WeaponBase.h"
#include "DrawDebugHelpers.h"
#include "InputActionValue.h"

UPlayerCombatComponent::UPlayerCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.SetTickFunctionEnable(false);
	
}

void UPlayerCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<APlayerCharacterBase>(GetOwner());
	
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCombatComponent : OwnerCharacter is nullptr"));
		return;
	}
	
	EquipWeapon(DefaultWeaponClass);
}

void UPlayerCombatComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(
		DeltaTime,
		TickType,
		ThisTickFunction);

	if (!bWeaponHitCheck)
		return;

	// WeaponTrace();
}

void UPlayerCombatComponent::EquipWeapon(TSubclassOf<AWeaponBase> WeaponClass)
{
	if (!OwnerCharacter)
	{
		return;
	}

	if (!WeaponClass)
	{
		return;
	}

	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
	}

	EquippedWeapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass);

	if (!EquippedWeapon)
	{
		return;
	}

	EquippedWeapon->AttachToComponent(
		OwnerCharacter->GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		WeaponSocketName);
}

void UPlayerCombatComponent::CacheWeaponTraceLocation()
{
	if (!EquippedWeapon)
	{
		return;
	}

	PreviousBladeStart = EquippedWeapon->GetBladeStartLocation();
	PreviousBladeEnd = EquippedWeapon->GetBladeEndLocation();
}

void UPlayerCombatComponent::StartWeaponHitCheck()
{
	if (!EquippedWeapon)
	{
		return;
	}

	HitActors.Empty();
	CacheWeaponTraceLocation();
	bWeaponHitCheck = true;
	SetComponentTickEnabled(true);
}

void UPlayerCombatComponent::EndWeaponHitCheck()
{
	bWeaponHitCheck = false;
	SetComponentTickEnabled(false);
}

void UPlayerCombatComponent::ProcessHit(const FHitResult& Hit)
{
	AActor* HitActor = Hit.GetActor();

	if (!HitActor)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Hit : %s"), *HitActor->GetName());

	// TODO
	// 데미지 주고받기
}

void UPlayerCombatComponent::WeaponTrace()
{
	if (!EquippedWeapon)
	{
		return;
	}

	FVector CurrentBladeStart =
		EquippedWeapon->GetBladeStartLocation();

	FVector CurrentBladeEnd =
		EquippedWeapon->GetBladeEndLocation();

	FCollisionShape CollisionShape =
		FCollisionShape::MakeSphere(TraceRadius);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	Params.AddIgnoredActor(EquippedWeapon);

	TArray<FHitResult> HitResults;
	
	GetWorld()->SweepMultiByChannel(
		HitResults,
		PreviousBladeStart,
		CurrentBladeStart,
		FQuat::Identity,
		TraceChannel,
		CollisionShape,
		Params);
	
	GetWorld()->SweepMultiByChannel(
		HitResults,
		PreviousBladeEnd,
		CurrentBladeEnd,
		FQuat::Identity,
		TraceChannel,
		CollisionShape,
		Params);
	
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();

		if (!HitActor)
		{
			continue;
		}

		if (HitActors.Contains(HitActor))
		{
			continue;
		}

		HitActors.Add(HitActor);

		ProcessHit(Hit);
	}
	
	PreviousBladeStart = CurrentBladeStart;
	PreviousBladeEnd = CurrentBladeEnd;
}

void UPlayerCombatComponent::Attack(const FInputActionValue& value)
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

void UPlayerCombatComponent::HeavyAttack(const FInputActionValue& value)
{
	if (!CanAttack())
	{
		return;
	}

	StartAttack(EAttackType::Heavy);
}

void UPlayerCombatComponent::Dodge(const FInputActionValue& value)
{
	if (!CanDodge())
	{
		return;
	}

	OwnerCharacter->GetStateTagComponent()->AddStateTag(CombatTags::State_Combat_Dodging);

	OwnerCharacter->PlayAnimMontage(DodgeMontage);
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

	EndWeaponHitCheck();
	
	OwnerCharacter->GetStateTagComponent()->RemoveStateTag(CombatTags::State_Combat_Attacking);
	
}

void UPlayerCombatComponent::ContinueCombo()
{
	bComboBuffered = false;

	++ComboIndex;

	if (ComboIndex >= ComboSectionNames.Num())
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

void UPlayerCombatComponent::EnableInvincible()
{
	bInvincible = true;
	
	OwnerCharacter->GetStateTagComponent()->AddStateTag(CombatTags::State_Combat_Invincible);
}

void UPlayerCombatComponent::DisableInvincible()
{
	bInvincible = false;

	if (OwnerCharacter->GetStateTagComponent()->HasStateTag(CombatTags::State_Combat_Invincible))
	{
		OwnerCharacter->GetStateTagComponent()->RemoveStateTag(CombatTags::State_Combat_Invincible);
	}
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