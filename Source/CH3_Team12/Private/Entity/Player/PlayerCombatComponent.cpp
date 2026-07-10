#include "Entity/Player/PlayerCombatComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/StateTagComponent.h"
#include "Entity/Player/PlayerLocomotionComponent.h"
#include "Entity/Player/PlayerAttributeComponent.h"
#include "Entity/Player/PlayerEquipmentComponent.h"
#include "GameplayTags/CombatGameplayTags.h"

#include "InputActionValue.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Entity/Weapon/WeaponBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"

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
}

EDefenseResult UPlayerCombatComponent::ResolveIncomingAttack(
	const FIncomingAttackContext& Context)
{
	if (!OwnerCharacter || !StateComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("ResolveIncomingAttack: None"));
		return EDefenseResult::None;
	}

	if (AttributeComponent && AttributeComponent->IsDead())
	{
		UE_LOG(LogTemp, Warning, TEXT("ResolveIncomingAttack: Dead"));
		return EDefenseResult::None;
	}

	if (StateComponent->HasStateTagExact(
		CombatTags::State_Combat_Invincible))
	{
		UE_LOG(LogTemp, Warning, TEXT("ResolveIncomingAttack: Invincible"));
		return EDefenseResult::Invincible;
	}

	const EHitReactionDirection ReactionDirection =
		CalculateHitReactionDirection(Context);

	if (IsParrying() && Context.AttackInfo.bCanBeParried)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ResolveIncomingAttack: Parry / Direction: %s"),
			*UEnum::GetValueAsString(ReactionDirection)
		);

		HandleParrySuccess(Context, ReactionDirection);
		return EDefenseResult::Parry;
	}

	if (IsGuarding() && Context.AttackInfo.bCanBeGuarded)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("ResolveIncomingAttack: Guard / Direction: %s"),
			*UEnum::GetValueAsString(ReactionDirection)
		);

		HandleGuardSuccess(Context, ReactionDirection);
		return EDefenseResult::Guard;
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("ResolveIncomingAttack: Hit / Direction: %s"),
		*UEnum::GetValueAsString(ReactionDirection)
	);

	HandleDirectHit(Context, ReactionDirection);
	return EDefenseResult::Hit;
}

bool UPlayerCombatComponent::IsGuarding() const
{
	return StateComponent &&
		StateComponent->HasStateTagExact(
			CombatTags::State_Combat_Guarding
		);
}

bool UPlayerCombatComponent::IsParrying() const
{
	return StateComponent &&
		StateComponent->HasStateTagExact(
			CombatTags::State_Combat_Parry
		);
}

void UPlayerCombatComponent::CacheWeaponTraceLocation()
{
	if (!EquipmentComponent->GetEquippedWeapon())
	{
		return;
	}

	PreviousBladeStart = EquipmentComponent->GetEquippedWeapon()->GetBladeStartLocation();
	PreviousBladeEnd = EquipmentComponent->GetEquippedWeapon()->GetBladeEndLocation();
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

	if (MontageData->GuardStartMontage)
	{
		OwnerCharacter->PlayAnimMontage(MontageData->GuardStartMontage);
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

	if (MontageData->GuardStartMontage)
	{
		if (USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh())
		{
			if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
			{
				AnimInstance->Montage_Stop(
					GuardMontageBlendOutTime,
					MontageData->GuardStartMontage
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

	if (!EquipmentComponent->GetEquippedWeapon())
	{
		return false;
	}

	if (!StateComponent->HasStateTagExact(CombatTags::State_Combat_Armed))
	{
		return false;
	}
	
	FGameplayTagContainer BlockTags;
	BlockTags.AddTag(CombatTags::State_Combat_Attacking);
	BlockTags.AddTag(CombatTags::State_Combat_Dodging);
	BlockTags.AddTag(CombatTags::State_Combat_Parry);
	BlockTags.AddTag(CombatTags::State_Movement_Locked);
	BlockTags.AddTag(CombatTags::State_Hit_PostureBroken);
	BlockTags.AddTag(CombatTags::State_Hit_Dead);
	BlockTags.AddTag(CombatTags::State_Hit_Reacting);

	return !StateComponent->HasAnyStateTags(BlockTags);
}

void UPlayerCombatComponent::PlayParryReaction(
	EHitReactionDirection AttackDirection)
{
	UAnimMontage* MontageToPlay = nullptr;

	switch (AttackDirection)
	{
	case EHitReactionDirection::Left:
		MontageToPlay = MontageData->ParryLeftMontage;
		break;

	case EHitReactionDirection::Right:
		MontageToPlay = MontageData->ParryRightMontage;
		break;

	default:
		MontageToPlay = MontageData->ParryRightMontage
			? MontageData->ParryRightMontage
			: MontageData->ParryLeftMontage;
		break;
	}

	if (MontageToPlay && OwnerCharacter)
	{
		OwnerCharacter->PlayAnimMontage(MontageToPlay);
	}
}

void UPlayerCombatComponent::PlayGuardHitReaction(
	EHitReactionDirection AttackDirection)
{
	UAnimMontage* MontageToPlay = nullptr;

	switch (AttackDirection)
	{
	case EHitReactionDirection::Left:
		MontageToPlay = MontageData->GuardHitLeftMontage;
		break;

	case EHitReactionDirection::Right:
		MontageToPlay = MontageData->GuardHitRightMontage;
		break;

	default:
		MontageToPlay = MontageData->GuardHitRightMontage
			? MontageData->GuardHitRightMontage
			: MontageData->GuardHitLeftMontage;
		break;
	}

	if (MontageToPlay && OwnerCharacter)
	{
		OwnerCharacter->PlayAnimMontage(MontageToPlay);
	}
}

UAnimMontage* UPlayerCombatComponent::GetHitMontage(
	EHitReactionDirection ReactionDirection) const
{
	if (!MontageData)
	{
		return nullptr;
	}

	switch (ReactionDirection)
	{
	case EHitReactionDirection::Left:
		return MontageData->HitLeftMontage
			? MontageData->HitLeftMontage
			: MontageData->HitFrontMontage;

	case EHitReactionDirection::Right:
		return MontageData->HitRightMontage
			? MontageData->HitRightMontage
			: MontageData->HitFrontMontage;

	case EHitReactionDirection::Back:
		return MontageData->HitBackMontage
			? MontageData->HitBackMontage
			: MontageData->HitFrontMontage;

	case EHitReactionDirection::Front:
	default:
		return MontageData->HitFrontMontage;
	}
}

void UPlayerCombatComponent::PlayHitReaction(
	EHitReactionDirection ReactionDirection)
{
	UAnimMontage* MontageToPlay = GetHitMontage(ReactionDirection);
	
	if (MontageToPlay && OwnerCharacter)
	{
		OwnerCharacter->PlayAnimMontage(MontageToPlay);
	}
	
	UAnimInstance* AnimInstance =
		OwnerCharacter->GetMesh()
			? OwnerCharacter->GetMesh()->GetAnimInstance()
			: nullptr;

	if (!AnimInstance)
	{
		return;
	}
	
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(
		this,
		&UPlayerCombatComponent::OnHitReactionMontageEnded
	);

	AnimInstance->Montage_SetEndDelegate(
		EndDelegate,
		MontageToPlay
	);
}

void UPlayerCombatComponent::OnHitReactionMontageEnded(
	UAnimMontage* Montage,
	bool bInterrupted)
{
	EndHitReaction();
}

void UPlayerCombatComponent::EndHitReaction()
{
	if (StateComponent)
	{
		StateComponent->RemoveStateTag(CombatTags::State_Hit_Reacting);
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

EHitReactionDirection UPlayerCombatComponent::CalculateHitReactionDirection(
	const FIncomingAttackContext& Context) const
{
	if (!OwnerCharacter)
	{
		return EHitReactionDirection::Front;
	}

	// 1순위: Hit 위치 기준
	if (!Context.Hit.ImpactPoint.IsNearlyZero())
	{
		FVector ToHit =
			Context.Hit.ImpactPoint - OwnerCharacter->GetActorLocation();

		ToHit.Z = 0.0f;

		if (!ToHit.IsNearlyZero())
		{
			ToHit.Normalize();

			const float RightDot = FVector::DotProduct(
				OwnerCharacter->GetActorRightVector(),
				ToHit
			);

			const float ForwardDot = FVector::DotProduct(
				OwnerCharacter->GetActorForwardVector(),
				ToHit
			);

			if (FMath::Abs(RightDot) > FMath::Abs(ForwardDot))
			{
				return RightDot > 0.0f
					? EHitReactionDirection::Right
					: EHitReactionDirection::Left;
			}

			return ForwardDot >= 0.0f
				? EHitReactionDirection::Front
				: EHitReactionDirection::Back;
		}
	}

	// 2순위: Attacker 위치 기준
	if (Context.Attacker)
	{
		FVector ToAttacker =
			Context.Attacker->GetActorLocation()
			- OwnerCharacter->GetActorLocation();

		ToAttacker.Z = 0.0f;

		if (!ToAttacker.IsNearlyZero())
		{
			ToAttacker.Normalize();

			const float RightDot = FVector::DotProduct(
				OwnerCharacter->GetActorRightVector(),
				ToAttacker
			);

			const float ForwardDot = FVector::DotProduct(
				OwnerCharacter->GetActorForwardVector(),
				ToAttacker
			);

			if (FMath::Abs(RightDot) > FMath::Abs(ForwardDot))
			{
				return RightDot > 0.0f
					? EHitReactionDirection::Right
					: EHitReactionDirection::Left;
			}

			return ForwardDot >= 0.0f
				? EHitReactionDirection::Front
				: EHitReactionDirection::Back;
		}
	}

	return EHitReactionDirection::Front;
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
	if (!EquipmentComponent->GetEquippedWeapon())
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
	if (!bWeaponHitCheck || !EquipmentComponent->GetEquippedWeapon() || !OwnerCharacter)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector CurrentBladeStart =
		EquipmentComponent->GetEquippedWeapon()->GetBladeStartLocation();

	const FVector CurrentBladeEnd =
		EquipmentComponent->GetEquippedWeapon()->GetBladeEndLocation();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	Params.AddIgnoredActor(EquipmentComponent->GetEquippedWeapon());

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
		AttackMontage = MontageData->LightAttackMontage;
		break;

	case EAttackType::Heavy:
		AttackMontage = MontageData->HeavyAttackMontage;
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
			? MontageData->LightAttackMontage
			: MontageData->HeavyAttackMontage;

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

void UPlayerCombatComponent::Debug_ReceiveTestAttackFront()
{
	Debug_ReceiveTestAttack(EHitReactionDirection::Front);
}

void UPlayerCombatComponent::Debug_ReceiveTestAttackLeft()
{
	Debug_ReceiveTestAttack(EHitReactionDirection::Left);
}

void UPlayerCombatComponent::Debug_ReceiveTestAttackRight()
{
	Debug_ReceiveTestAttack(EHitReactionDirection::Right);
}

void UPlayerCombatComponent::Debug_ReceiveTestAttackBack()
{
	Debug_ReceiveTestAttack(EHitReactionDirection::Back);
}

void UPlayerCombatComponent::Debug_ReceiveTestAttack(
	EHitReactionDirection Direction)
{
	if (!OwnerCharacter)
	{
		return;
	}

	FIncomingAttackContext Context;

	Context.Attacker = nullptr;
	Context.AttackInfo.Damage = DebugAttackDamage;
	Context.AttackInfo.PostureDamage = DebugAttackPostureDamage;
	Context.AttackInfo.SwingDirection = EAttackSwingDirection::None;
	Context.AttackInfo.bCanBeParried = true;
	Context.AttackInfo.bCanBeGuarded = true;

	const FVector OwnerLocation =
		OwnerCharacter->GetActorLocation();

	const FVector Forward =
		OwnerCharacter->GetActorForwardVector();

	const FVector Right =
		OwnerCharacter->GetActorRightVector();

	FVector HitDirection = Forward;

	switch (Direction)
	{
	case EHitReactionDirection::Left:
		HitDirection = -Right;
		break;

	case EHitReactionDirection::Right:
		HitDirection = Right;
		break;

	case EHitReactionDirection::Back:
		HitDirection = -Forward;
		break;

	case EHitReactionDirection::Front:
	default:
		HitDirection = Forward;
		break;
	}

	Context.Hit.ImpactPoint =
		OwnerLocation;

	Context.AttackWorldDirection =
		-HitDirection;

	const EDefenseResult Result =
		ResolveIncomingAttack(Context);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Debug Test Attack Result: %s"),
		*UEnum::GetValueAsString(Result)
	);
}

void UPlayerCombatComponent::PlayCombatFeedback(
	const FIncomingAttackContext& Context,
	const FCombatFeedbackData& Feedback)
{
	const FVector Location =
		MakeCombatEffectLocation(Context, Feedback.LocationMode);

	const FRotator Rotation =
		MakeCombatEffectRotation(Context, Feedback.RotationMode);

	if (Feedback.Effect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			Feedback.Effect,
			Location,
			Rotation
		);
	}

	if (Feedback.Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			Feedback.Sound,
			Location
		);
	}

	TriggerCombatHitStop(
		Context,
		Feedback.HitStopDuration,
		Feedback.HitStopTimeDilation
	);
}

void UPlayerCombatComponent::HandleParrySuccess(
	const FIncomingAttackContext& Context,
	EHitReactionDirection ReactionDirection)
{
	PlayParryReaction(ReactionDirection);

	if (FeedbackData)
	{
		PlayCombatFeedback(
			Context,
			FeedbackData->ParryFeedback
		);
	}
}

void UPlayerCombatComponent::HandleGuardSuccess(
	const FIncomingAttackContext& Context,
	EHitReactionDirection ReactionDirection)
{
	PlayGuardHitReaction(ReactionDirection);

	if (AttributeComponent)
	{
		const float ChipDamage =
			Context.AttackInfo.Damage * GuardChipDamageRate;

		const float GuardPostureDamage =
			Context.AttackInfo.PostureDamage * GuardPostureDamageRate;

		AttributeComponent->ApplyAttributeDamage(
			ChipDamage,
			GuardPostureDamage
		);
	}

	if (FeedbackData)
	{
		PlayCombatFeedback(
			Context,
			FeedbackData->GuardFeedback
		);
	}
}

void UPlayerCombatComponent::HandleDirectHit(
	const FIncomingAttackContext& Context,
	EHitReactionDirection ReactionDirection)
{
	if (StateComponent)
	{
		StateComponent->AddStateTag(CombatTags::State_Hit_Reacting);
		StateComponent->AddStateTag(CombatTags::State_Movement_Locked);

		StateComponent->RemoveStateTag(CombatTags::State_Combat_Attacking);
		StateComponent->RemoveStateTag(CombatTags::State_Combat_Guarding);
		StateComponent->RemoveStateTag(CombatTags::State_Combat_Parry);
	}

	EndWeaponHitCheck();

	PlayHitReaction(ReactionDirection);

	if (AttributeComponent)
	{
		AttributeComponent->ApplyHealthDamage(
			Context.AttackInfo.Damage
		);
	}

	if (FeedbackData)
	{
		PlayCombatFeedback(
			Context,
			FeedbackData->HitFeedback
		);
	}
}

void UPlayerCombatComponent::TriggerCombatHitStop(
	const FIncomingAttackContext& Context,
	float Duration,
	float TimeDilation)
{
	if (!GetWorld())
	{
		return;
	}

	if (Duration <= 0.0f)
	{
		return;
	}

	TimeDilation = FMath::Clamp(
		TimeDilation,
		0.01f,
		1.0f
	);

	// 이전 HitStop이 남아 있으면 먼저 원복
	ResetCombatHitStop();

	HitStopActors.Reset();

	if (OwnerCharacter)
	{
		HitStopActors.Add(OwnerCharacter);
	}

	if (Context.Attacker)
	{
		HitStopActors.Add(Context.Attacker);
	}

	for (TWeakObjectPtr<AActor> ActorPtr : HitStopActors)
	{
		if (AActor* Actor = ActorPtr.Get())
		{
			Actor->CustomTimeDilation = TimeDilation;
		}
	}

	GetWorld()->GetTimerManager().SetTimer(
		HitStopTimerHandle,
		this,
		&UPlayerCombatComponent::ResetCombatHitStop,
		Duration,
		false
	);
}

void UPlayerCombatComponent::ResetCombatHitStop()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(
			HitStopTimerHandle
		);
	}

	for (TWeakObjectPtr<AActor> ActorPtr : HitStopActors)
	{
		if (AActor* Actor = ActorPtr.Get())
		{
			Actor->CustomTimeDilation = 1.0f;
		}
	}

	HitStopActors.Reset();
}

FVector UPlayerCombatComponent::GetWeaponClashEffectLocation(
	const FIncomingAttackContext& Context) const
{
	if (EquipmentComponent->GetEquippedWeapon())
	{
		if (UStaticMeshComponent* WeaponMesh =
			EquipmentComponent->GetEquippedWeapon()->GetWeaponMesh())
		{
			if (WeaponMesh->DoesSocketExist(FeedbackData->WeaponClashEffectSocketName))
			{
				return WeaponMesh->GetSocketLocation(
					FeedbackData->WeaponClashEffectSocketName
				);
			}
		}

		const FVector BladeStart =
			EquipmentComponent->GetEquippedWeapon()->GetBladeStartLocation();

		const FVector BladeEnd =
			EquipmentComponent->GetEquippedWeapon()->GetBladeEndLocation();

		if (!BladeStart.IsNearlyZero() && !BladeEnd.IsNearlyZero())
		{
			return (BladeStart + BladeEnd) * 0.5f;
		}
	}

	return GetHitImpactEffectLocation(Context);
}

FVector UPlayerCombatComponent::GetHitImpactEffectLocation(
	const FIncomingAttackContext& Context) const
{
	if (!Context.Hit.ImpactPoint.IsNearlyZero())
	{
		if (!Context.Hit.ImpactNormal.IsNearlyZero())
		{
			return Context.Hit.ImpactPoint
				+ Context.Hit.ImpactNormal.GetSafeNormal()
				* FeedbackData->HitEffectSurfaceOffset;
		}

		return Context.Hit.ImpactPoint;
	}

	return GetFallbackEffectLocation();
}

FVector UPlayerCombatComponent::GetFallbackEffectLocation() const
{
	if (!OwnerCharacter)
	{
		return FVector::ZeroVector;
	}

	return OwnerCharacter->GetActorLocation()
		+ FVector(0.0f, 0.0f, FeedbackData->FallbackEffectHeightOffset);
}

FVector UPlayerCombatComponent::MakeCombatEffectLocation(
	const FIncomingAttackContext& Context,
	ECombatEffectLocationMode LocationMode) const
{
	switch (LocationMode)
	{
	case ECombatEffectLocationMode::DefenderWeaponClashSocket:
		return GetWeaponClashEffectLocation(Context);

	case ECombatEffectLocationMode::DefenderWeaponBladeMiddle:
		if (EquipmentComponent->GetEquippedWeapon())
		{
			const FVector BladeStart =
				EquipmentComponent->GetEquippedWeapon()->GetBladeStartLocation();

			const FVector BladeEnd =
				EquipmentComponent->GetEquippedWeapon()->GetBladeEndLocation();

			if (!BladeStart.IsNearlyZero() && !BladeEnd.IsNearlyZero())
			{
				return (BladeStart + BladeEnd) * 0.5f;
			}
		}

		return GetWeaponClashEffectLocation(Context);

	case ECombatEffectLocationMode::DefenderActorCenter:
		return GetFallbackEffectLocation();

	case ECombatEffectLocationMode::HitImpactPoint:
	default:
		return GetHitImpactEffectLocation(Context);
	}
}

FRotator UPlayerCombatComponent::MakeCombatEffectRotation(
	const FIncomingAttackContext& Context,
	ECombatEffectRotationMode RotationMode) const
{
	if (!OwnerCharacter)
	{
		return FRotator::ZeroRotator;
	}

	FVector Direction = OwnerCharacter->GetActorForwardVector();

	switch (RotationMode)
	{
	case ECombatEffectRotationMode::ImpactNormal:
		if (!Context.Hit.ImpactNormal.IsNearlyZero())
		{
			Direction = Context.Hit.ImpactNormal;
		}
		break;

	case ECombatEffectRotationMode::AttackDirection:
		if (!Context.AttackWorldDirection.IsNearlyZero())
		{
			Direction = Context.AttackWorldDirection;
		}
		break;

	case ECombatEffectRotationMode::OppositeAttackDirection:
		if (!Context.AttackWorldDirection.IsNearlyZero())
		{
			Direction = -Context.AttackWorldDirection;
		}
		break;

	case ECombatEffectRotationMode::AttackerToDefender:
		if (Context.Attacker)
		{
			Direction =
				OwnerCharacter->GetActorLocation()
				- Context.Attacker->GetActorLocation();
		}
		break;

	case ECombatEffectRotationMode::DefenderToAttacker:
		if (Context.Attacker)
		{
			Direction =
				Context.Attacker->GetActorLocation()
				- OwnerCharacter->GetActorLocation();
		}
		break;

	case ECombatEffectRotationMode::DefenderForward:
		Direction = OwnerCharacter->GetActorForwardVector();
		break;

	case ECombatEffectRotationMode::None:
	default:
		return FRotator::ZeroRotator;
	}

	if (Direction.IsNearlyZero())
	{
		return FRotator::ZeroRotator;
	}

	return FRotationMatrix::MakeFromX(
		Direction.GetSafeNormal()
	).Rotator();
}