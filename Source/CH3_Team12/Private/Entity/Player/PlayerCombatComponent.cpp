#include "Entity/Player/PlayerCombatComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/StateTagComponent.h"
#include "Entity/Player/PlayerLocomotionComponent.h"
#include "GameplayTags/CombatGameplayTags.h"
#include "InputActionValue.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Entity/Weapon/WeaponBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"

UPlayerCombatComponent::UPlayerCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
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

	StateComponent = OwnerCharacter->GetStateTagComponent();
	if (!StateComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCombatComponent : StateComponent is nullptr"));
		return;
	}

	EquipWeapon(DefaultWeaponClass);
}

void UPlayerCombatComponent::EquipWeapon(TSubclassOf<AWeaponBase> WeaponClass)
{
	if (!OwnerCharacter || !WeaponClass)
	{
		return;
	}

	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwnerCharacter;
	SpawnParams.Instigator = OwnerCharacter;

	EquippedWeapon = World->SpawnActor<AWeaponBase>(
		WeaponClass,
		OwnerCharacter->GetActorLocation(),
		OwnerCharacter->GetActorRotation(),
		SpawnParams
	);

	if (!EquippedWeapon)
	{
		return;
	}

	EquippedWeapon->AttachToComponent(
		OwnerCharacter->GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		WeaponSocketName
	);

	if (StateComponent)
	{
		StateComponent->AddStateTag(CombatTags::State_Combat_Armed);
	}
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

void UPlayerCombatComponent::OnAttackMontageEnded(
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

void UPlayerCombatComponent::StartGuard(const FInputActionValue& Value)
{
	if (!CanGuard())
	{
		return;
	}

	if (!OwnerCharacter || !StateComponent)
	{
		return;
	}

	if (UPlayerLocomotionComponent* LocomotionComponent =
		OwnerCharacter->GetLocomotionComponent())
	{
		LocomotionComponent->DoStopSprint();
	}

	StateComponent->AddStateTag(
		CombatTags::State_Combat_Guarding
	);

	if (UPlayerLocomotionComponent* LocomotionComponent =
		OwnerCharacter->GetLocomotionComponent())
	{
		LocomotionComponent->RefreshMovementSettings();
	}

	if (GuardStartMontage)
	{
		OwnerCharacter->PlayAnimMontage(GuardStartMontage);
	}
}

void UPlayerCombatComponent::StopGuard(const FInputActionValue& Value)
{
	if (!OwnerCharacter || !StateComponent)
	{
		return;
	}

	StateComponent->RemoveStateTag(
		CombatTags::State_Combat_Guarding
	);

	StateComponent->RemoveStateTag(
		CombatTags::State_Combat_Parry
	);

	if (GuardStartMontage)
	{
		if (USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh())
		{
			if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
			{
				AnimInstance->Montage_Stop(
					GuardMontageBlendOutTime,
					GuardStartMontage
				);
			}
		}
	}

	if (UPlayerLocomotionComponent* LocomotionComponent =
		OwnerCharacter->GetLocomotionComponent())
	{
		LocomotionComponent->RefreshMovementSettings();
	}
}

void UPlayerCombatComponent::OpenParryWindow()
{
	if (!StateComponent)
	{
		return;
	}

	if (!StateComponent->HasStateTagExact(
		CombatTags::State_Combat_Guarding))
	{
		return;
	}

	StateComponent->AddStateTag(
		CombatTags::State_Combat_Parry
	);
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,                                   // Key (화면 덮어쓰기 키)
			2.0f,                                 // 화면에 떠 있을 시간 (초)
			FColor::Red,                          // 텍스트 색상
		   FString(TEXT("Open Parry Window"))
		);
	}
}

void UPlayerCombatComponent::CloseParryWindow()
{
	if (!StateComponent)
	{
		return;
	}

	StateComponent->RemoveStateTag(
		CombatTags::State_Combat_Parry
	);
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
		   -1, 
		   2.0f, 
		   FColor::Red, 
		   FString(TEXT("Close Parry Window"))
		);
	}
}

bool UPlayerCombatComponent::CanGuard() const
{
	if (!OwnerCharacter || !StateComponent)
	{
		return false;
	}

	if (!EquippedWeapon)
	{
		return false;
	}

	if (!StateComponent->HasStateTagExact(
		CombatTags::State_Combat_Armed))
	{
		return false;
	}

	FGameplayTagContainer BlockTags;
	BlockTags.AddTag(CombatTags::State_Combat_Attacking);
	BlockTags.AddTag(CombatTags::State_Combat_Dodging);
	BlockTags.AddTag(CombatTags::State_Hit_PostureBroken);
	BlockTags.AddTag(CombatTags::State_Hit_Dead);

	return !StateComponent->HasAnyStateTags(BlockTags);
}

void UPlayerCombatComponent::OpenAttackRecovery()
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
		LocomotionComponent->OpenDodgeBufferWindow();
		LocomotionComponent->RefreshMovementSettings();
	}
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
}

void UPlayerCombatComponent::EndWeaponHitCheck()
{
	bWeaponHitCheck = false;

	HitActors.Empty();

	PreviousBladeStart = FVector::ZeroVector;
	PreviousBladeEnd = FVector::ZeroVector;
}

void UPlayerCombatComponent::EnableInvincible()
{
	if (!StateComponent)
	{
		return;
	}

	bInvincible = true;

	StateComponent->AddStateTag(
		CombatTags::State_Combat_Invincible
	);
}

void UPlayerCombatComponent::DisableInvincible()
{
	if (!StateComponent)
	{
		return;
	}

	bInvincible = false;

	StateComponent->RemoveStateTag(
		CombatTags::State_Combat_Invincible
	);
}

void UPlayerCombatComponent::ProcessHit(const FHitResult& Hit)
{
	AActor* HitActor = Hit.GetActor();
	if (!HitActor || !OwnerCharacter)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Hit : %s"), *HitActor->GetName());

	UGameplayStatics::ApplyDamage(
		HitActor,
		25.0f,
		OwnerCharacter->GetController(),
		OwnerCharacter,
		nullptr
	);
}

void UPlayerCombatComponent::WeaponTrace()
{
	if (!bWeaponHitCheck || !EquippedWeapon || !OwnerCharacter)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector CurrentBladeStart =
		EquippedWeapon->GetBladeStartLocation();

	const FVector CurrentBladeEnd =
		EquippedWeapon->GetBladeEndLocation();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	Params.AddIgnoredActor(EquippedWeapon);

	const FCollisionShape CollisionShape =
		FCollisionShape::MakeSphere(TraceRadius);

	const int32 SafeSampleCount = FMath::Max(TraceSampleCount, 2);

	for (int32 Index = 0; Index < SafeSampleCount; ++Index)
	{
		const float Alpha =
			static_cast<float>(Index) /
			static_cast<float>(SafeSampleCount - 1);

		const FVector PreviousPoint =
			FMath::Lerp(PreviousBladeStart, PreviousBladeEnd, Alpha);

		const FVector CurrentPoint =
			FMath::Lerp(CurrentBladeStart, CurrentBladeEnd, Alpha);

		TArray<FHitResult> HitResults;

		const bool bHit = World->SweepMultiByChannel(
			HitResults,
			PreviousPoint,
			CurrentPoint,
			FQuat::Identity,
			TraceChannel,
			CollisionShape,
			Params
		);

		if (!bHit)
		{
			continue;
		}

		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();

			if (!HitActor || HitActor == OwnerCharacter)
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
	}

	PreviousBladeStart = CurrentBladeStart;
	PreviousBladeEnd = CurrentBladeEnd;
}

void UPlayerCombatComponent::Attack(const FInputActionValue& Value)
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

void UPlayerCombatComponent::HeavyAttack(const FInputActionValue& Value)
{
	if (!CanAttack())
	{
		return;
	}

	StartAttack(EAttackType::Heavy);
}

bool UPlayerCombatComponent::CanAttack() const
{
	if (!OwnerCharacter || !StateComponent)
	{
		return false;
	}

	if (!EquippedWeapon)
	{
		return false;
	}

	if (!StateComponent->HasStateTagExact(CombatTags::State_Combat_Armed))
	{
		return false;
	}

	if (StateComponent->HasStateTagExact(CombatTags::State_Combat_Dodging))
	{
		return false;
	}

	if (StateComponent->HasStateTagExact(CombatTags::State_Combat_Guarding))
	{
		return false;
	}

	if (StateComponent->HasStateTagExact(CombatTags::State_Hit_Dead))
	{
		return false;
	}

	return true;
}

bool UPlayerCombatComponent::IsAttacking() const
{
	if (!StateComponent)
	{
		return false;
	}

	return StateComponent->HasStateTagExact(
		CombatTags::State_Combat_Attacking
	);
}

bool UPlayerCombatComponent::IsBusy() const
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

bool UPlayerCombatComponent::CanContinueCombo() const
{
	const int32 NextComboIndex = ComboIndex + 1;
	return ComboSectionNames.IsValidIndex(NextComboIndex);
}

void UPlayerCombatComponent::StartAttack(EAttackType AttackType)
{
	if (!OwnerCharacter || !StateComponent)
	{
		return;
	}

	UAnimMontage* AttackMontage = nullptr;

	switch (AttackType)
	{
	case EAttackType::Light:
		AttackMontage = LightAttackMontage;
		break;

	case EAttackType::Heavy:
		AttackMontage = HeavyAttackMontage;
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
		AnimInstance->Montage_Play(AttackMontage, AttackPlayRate);

	if (Duration <= 0.0f)
	{
		EndAttack();
		return;
	}

	AnimInstance->Montage_JumpToSection(
		ComboSectionNames[ComboIndex],
		AttackMontage
	);

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(
		this,
		&UPlayerCombatComponent::OnAttackMontageEnded
	);

	AnimInstance->Montage_SetEndDelegate(
		EndDelegate,
		AttackMontage
	);
}

void UPlayerCombatComponent::EndAttack()
{
	EndWeaponHitCheck();

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
			LocomotionComponent->CloseDodgeBufferWindow();
			LocomotionComponent->RefreshMovementSettings();
		}
	}
}

void UPlayerCombatComponent::OpenComboWindow()
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

void UPlayerCombatComponent::ContinueCombo()
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
			? LightAttackMontage
			: HeavyAttackMontage;

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
		ComboSectionNames[ComboIndex],
		CurrentMontage
	);
}