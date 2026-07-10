#include "Entity/Player/PlayerDefenseComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/StateTagComponent.h"
#include "Entity/Player/PlayerLocomotionComponent.h"
#include "Entity/Player/PlayerAttributeComponent.h"
#include "Entity/Player/PlayerEquipmentComponent.h"
#include "Entity/Player/PlayerWeaponComponent.h"
#include "GameplayTags/CombatGameplayTags.h"
#include "Entity/Weapon/WeaponBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UPlayerDefenseComponent::UPlayerDefenseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// Called when the game starts
void UPlayerDefenseComponent::BeginPlay()
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

void UPlayerDefenseComponent::StartGuard(const FInputActionValue& Value)
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

	if (DefenseData->GuardStartMontage)
	{
		OwnerCharacter->PlayAnimMontage(DefenseData->GuardStartMontage);
	}
}

void UPlayerDefenseComponent::StopGuard(const FInputActionValue& Value)
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

	if (DefenseData->GuardStartMontage)
	{
		if (USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh())
		{
			if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
			{
				AnimInstance->Montage_Stop(
					DefenseData->GuardMontageBlendOutTime,
					DefenseData->GuardStartMontage
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

bool UPlayerDefenseComponent::CanGuard() const
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


void UPlayerDefenseComponent::OpenParryWindow()
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

void UPlayerDefenseComponent::CloseParryWindow()
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

EDefenseResult UPlayerDefenseComponent::ResolveIncomingAttack(
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

bool UPlayerDefenseComponent::IsGuarding() const
{
	return StateComponent &&
		StateComponent->HasStateTagExact(
			CombatTags::State_Combat_Guarding
		);
}

bool UPlayerDefenseComponent::IsParrying() const
{
	return StateComponent &&
		StateComponent->HasStateTagExact(
			CombatTags::State_Combat_Parry
		);
}

void UPlayerDefenseComponent::PlayParryReaction(
	EHitReactionDirection AttackDirection)
{
	UAnimMontage* MontageToPlay = nullptr;

	switch (AttackDirection)
	{
	case EHitReactionDirection::Left:
		MontageToPlay = DefenseData->ParryLeftMontage;
		break;

	case EHitReactionDirection::Right:
		MontageToPlay = DefenseData->ParryRightMontage;
		break;

	default:
		MontageToPlay = DefenseData->ParryRightMontage
			? DefenseData->ParryRightMontage
			: DefenseData->ParryLeftMontage;
		break;
	}

	if (MontageToPlay && OwnerCharacter)
	{
		OwnerCharacter->PlayAnimMontage(MontageToPlay);
	}
}

void UPlayerDefenseComponent::PlayGuardHitReaction(
	EHitReactionDirection AttackDirection)
{
	UAnimMontage* MontageToPlay = nullptr;

	switch (AttackDirection)
	{
	case EHitReactionDirection::Left:
		MontageToPlay = DefenseData->GuardHitLeftMontage;
		break;

	case EHitReactionDirection::Right:
		MontageToPlay = DefenseData->GuardHitRightMontage;
		break;

	default:
		MontageToPlay = DefenseData->GuardHitRightMontage
			? DefenseData->GuardHitRightMontage
			: DefenseData->GuardHitLeftMontage;
		break;
	}

	if (MontageToPlay && OwnerCharacter)
	{
		OwnerCharacter->PlayAnimMontage(MontageToPlay);
	}
}

UAnimMontage* UPlayerDefenseComponent::GetHitMontage(
	EHitReactionDirection ReactionDirection) const
{
	if (!DefenseData)
	{
		return nullptr;
	}

	switch (ReactionDirection)
	{
	case EHitReactionDirection::Left:
		return DefenseData->HitLeftMontage
			? DefenseData->HitLeftMontage
			: DefenseData->HitFrontMontage;

	case EHitReactionDirection::Right:
		return DefenseData->HitRightMontage
			? DefenseData->HitRightMontage
			: DefenseData->HitFrontMontage;

	case EHitReactionDirection::Back:
		return DefenseData->HitBackMontage
			? DefenseData->HitBackMontage
			: DefenseData->HitFrontMontage;

	case EHitReactionDirection::Front:
	default:
		return DefenseData->HitFrontMontage;
	}
}

void UPlayerDefenseComponent::PlayHitReaction(
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
		&UPlayerDefenseComponent::OnHitReactionMontageEnded
	);

	AnimInstance->Montage_SetEndDelegate(
		EndDelegate,
		MontageToPlay
	);
}

void UPlayerDefenseComponent::OnHitReactionMontageEnded(
	UAnimMontage* Montage,
	bool bInterrupted)
{
	EndHitReaction();
}

EHitReactionDirection UPlayerDefenseComponent::CalculateHitReactionDirection(
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

void UPlayerDefenseComponent::EndHitReaction()
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

void UPlayerDefenseComponent::HandleParrySuccess(
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

void UPlayerDefenseComponent::HandleGuardSuccess(
	const FIncomingAttackContext& Context,
	EHitReactionDirection ReactionDirection)
{
	PlayGuardHitReaction(ReactionDirection);

	if (AttributeComponent)
	{
		const float ChipDamage =
			Context.AttackInfo.Damage * DefenseData->GuardChipDamageRate;

		const float GuardPostureDamage =
			Context.AttackInfo.PostureDamage * DefenseData->GuardPostureDamageRate;

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

void UPlayerDefenseComponent::HandleDirectHit(
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

	WeaponComponent->EndWeaponHitCheck();

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

void UPlayerDefenseComponent::EnableInvincible()
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

void UPlayerDefenseComponent::DisableInvincible()
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

void UPlayerDefenseComponent::PlayCombatFeedback(
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


void UPlayerDefenseComponent::TriggerCombatHitStop(
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
		&UPlayerDefenseComponent::ResetCombatHitStop,
		Duration,
		false
	);
}

void UPlayerDefenseComponent::ResetCombatHitStop()
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

FVector UPlayerDefenseComponent::GetWeaponClashEffectLocation(
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

FVector UPlayerDefenseComponent::GetHitImpactEffectLocation(
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

FVector UPlayerDefenseComponent::GetFallbackEffectLocation() const
{
	if (!OwnerCharacter)
	{
		return FVector::ZeroVector;
	}

	return OwnerCharacter->GetActorLocation()
		+ FVector(0.0f, 0.0f, FeedbackData->FallbackEffectHeightOffset);
}

FVector UPlayerDefenseComponent::MakeCombatEffectLocation(
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

FRotator UPlayerDefenseComponent::MakeCombatEffectRotation(
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

void UPlayerDefenseComponent::Debug_ReceiveTestAttackFront()
{
	Debug_ReceiveTestAttack(EHitReactionDirection::Front);
}

void UPlayerDefenseComponent::Debug_ReceiveTestAttackLeft()
{
	Debug_ReceiveTestAttack(EHitReactionDirection::Left);
}

void UPlayerDefenseComponent::Debug_ReceiveTestAttackRight()
{
	Debug_ReceiveTestAttack(EHitReactionDirection::Right);
}

void UPlayerDefenseComponent::Debug_ReceiveTestAttackBack()
{
	Debug_ReceiveTestAttack(EHitReactionDirection::Back);
}

void UPlayerDefenseComponent::Debug_ReceiveTestAttack(
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
