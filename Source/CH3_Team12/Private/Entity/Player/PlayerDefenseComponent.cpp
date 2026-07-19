#include "Entity/Player/PlayerDefenseComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/StateTagComponent.h"
#include "Entity/Player/PlayerLocomotionComponent.h"
#include "Entity/Player/PlayerAttributeComponent.h"
#include "Entity/Player/PlayerEquipmentComponent.h"
#include "Entity/Player/PlayerWeaponComponent.h"
#include "Entity/Player/PlayerAttackComponent.h"
#include "Entity/Player/CombatFeedbackComponent.h"
#include "GameplayTags/CombatGameplayTags.h"
#include "Entity/Weapon/WeaponBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"

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
		UE_LOG(LogTemp, Error, TEXT("PlayerDefenseComponent : OwnerCharacter is nullptr"));
		return;
	}

	StateComponent = OwnerCharacter->GetStateTagComponent();
	AttributeComponent = OwnerCharacter->GetAttributeComponent();
	EquipmentComponent = OwnerCharacter->GetEquipmentComponent();
	WeaponComponent = OwnerCharacter->GetWeaponComponent();
	CombatFeedbackComponent = OwnerCharacter->GetCombatFeedbackComponent();

	if (!StateComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerDefenseComponent : StateComponent is nullptr"));
		return;
	}

	if (!AttributeComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerDefenseComponent : AttributeComponent is nullptr"));
		return;
	}

	if (!DefenseData)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerDefenseComponent : DefenseData is nullptr"));
		return;
	}

	if (!FeedbackData)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerDefenseComponent : FeedbackData is nullptr"));
	}

	if (AttributeComponent)
	{
		AttributeComponent->OnDead.AddDynamic(
			this,
			&UPlayerDefenseComponent::HandleOwnerDead
		);

		AttributeComponent->OnPostureBroken.AddDynamic(
			this,
			&UPlayerDefenseComponent::HandleOwnerPostureBroken
		);
		AttributeComponent->OnPostureRecovered.AddDynamic(
			this,
			&UPlayerDefenseComponent::HandleOwnerPostureRecovered
		);
	}
	
	if (!CombatFeedbackComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerDefenseComponent : CombatFeedbackComponent is nullptr"));
	}
}

void UPlayerDefenseComponent::EndPlay(
	const EEndPlayReason::Type EndPlayReason)
{
	if (AttributeComponent)
	{
		AttributeComponent->OnDead.RemoveDynamic(
			this,
			&UPlayerDefenseComponent::HandleOwnerDead
		);

		AttributeComponent->OnPostureBroken.RemoveDynamic(
			this,
			&UPlayerDefenseComponent::HandleOwnerPostureBroken
		);
		AttributeComponent->OnPostureRecovered.RemoveDynamic(
			this,
			&UPlayerDefenseComponent::HandleOwnerPostureRecovered
		);
	}

	Super::EndPlay(EndPlayReason);
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

	if (DefenseData && DefenseData->GuardStartMontage)
	{
		PlayMontageSafe(
			DefenseData->GuardStartMontage,
			DefenseData->GuardStartMontagePlayRate
		);
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

	if (DefenseData && DefenseData->GuardStartMontage)
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
			-1, // Key (화면 덮어쓰기 키)
			2.0f, // 화면에 떠 있을 시간 (초)
			FColor::Red, // 텍스트 색상
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
	if (!DefenseData)
	{
		return;
	}

	PlayMontageSafe(
		DefenseData->GetParryReactionMontage(AttackDirection),
		DefenseData->ParryReactionMontagePlayRate
	);
}

void UPlayerDefenseComponent::PlayGuardHitReaction(
	EHitReactionDirection AttackDirection)
{
	if (!DefenseData)
	{
		return;
	}

	PlayMontageSafe(
		DefenseData->GetGuardHitMontage(AttackDirection),
		DefenseData->GuardHitReactionMontagePlayRate
	);
}

void UPlayerDefenseComponent::PlayHitReaction(
	EHitReactionDirection ReactionDirection)
{
	if (!OwnerCharacter || !DefenseData)
	{
		EndHitReaction();
		return;
	}

	UAnimMontage* MontageToPlay =
		DefenseData->GetHitReactionMontage(ReactionDirection);

	const bool bPlayed =
		PlayMontageSafe(
			MontageToPlay,
			DefenseData->HitReactionMontagePlayRate
		);

	if (!bPlayed)
	{
		EndHitReaction();
		return;
	}

	UAnimInstance* AnimInstance =
		OwnerCharacter->GetMesh()
			? OwnerCharacter->GetMesh()->GetAnimInstance()
			: nullptr;

	if (!AnimInstance)
	{
		EndHitReaction();
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
	if (!StateComponent)
	{
		return;
	}

	if (StateComponent->HasStateTagExact(CombatTags::State_Hit_Dead))
	{
		StateComponent->RemoveStateTag(CombatTags::State_Hit_Reacting);
		return;
	}

	StateComponent->RemoveStateTag(CombatTags::State_Hit_Reacting);
	StateComponent->RemoveStateTag(CombatTags::State_Movement_Locked);

	if (OwnerCharacter)
	{
		if (UPlayerLocomotionComponent* LocomotionComponent =
			OwnerCharacter->GetLocomotionComponent())
		{
			LocomotionComponent->RefreshMovementSettings();
		}
	}
}

bool UPlayerDefenseComponent::PlayMontageSafe(
	UAnimMontage* Montage,
	float PlayRate) const
{
	if (!OwnerCharacter || !Montage)
	{
		return false;
	}

	const float SafePlayRate =
		FMath::Max(PlayRate, 0.01f);

	return OwnerCharacter->PlayAnimMontage(
		Montage,
		SafePlayRate
	) > 0.0f;
}

void UPlayerDefenseComponent::HandleParrySuccess(
	const FIncomingAttackContext& Context,
	EHitReactionDirection ReactionDirection)
{
	PlayParryReaction(ReactionDirection);

	if (CombatFeedbackComponent && FeedbackData)
	{
		CombatFeedbackComponent->PlayCombatFeedback(
			Context,
			FeedbackData,
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
			Context.AttackInfo.Damage *
			DefenseData->GuardChipDamageRate;

		const float GuardPostureDamage =
			Context.AttackInfo.PostureDamage *
			DefenseData->GuardPostureDamageRate;

		AttributeComponent->ApplyAttributeDamage(
			ChipDamage,
			GuardPostureDamage
		);
	}

	if (CombatFeedbackComponent && FeedbackData)
	{
		CombatFeedbackComponent->PlayCombatFeedback(
			Context,
			FeedbackData,
			FeedbackData->GuardFeedback
		);
	}
}

void UPlayerDefenseComponent::HandleDirectHit(
	const FIncomingAttackContext& Context,
	EHitReactionDirection ReactionDirection)
{
	if (!OwnerCharacter || !StateComponent)
	{
		return;
	}

	if (AttributeComponent && AttributeComponent->IsDead())
	{
		return;
	}

	StateComponent->AddStateTag(CombatTags::State_Hit_Reacting);
	StateComponent->AddStateTag(CombatTags::State_Movement_Locked);

	StateComponent->RemoveStateTag(CombatTags::State_Combat_Attacking);
	StateComponent->RemoveStateTag(CombatTags::State_Combat_Guarding);
	StateComponent->RemoveStateTag(CombatTags::State_Combat_Parry);

	if (WeaponComponent)
	{
		WeaponComponent->EndWeaponHitCheck();
	}

	if (AttributeComponent)
	{
		AttributeComponent->ApplyHealthDamage(
			Context.AttackInfo.Damage
		);

		// ApplyHealthDamage 안에서 Die() → OnDead.Broadcast()가 즉시 호출됨.
		// 죽었으면 일반 HitReaction으로 가지 않는다.
		if (AttributeComponent->IsDead())
		{
			if (CombatFeedbackComponent && FeedbackData)
			{
				CombatFeedbackComponent->PlayCombatFeedback(
					Context,
					FeedbackData,
					FeedbackData->HitFeedback
				);
			}

			return;
		}
	}

	PlayHitReaction(ReactionDirection);

	if (CombatFeedbackComponent && FeedbackData)
	{
		CombatFeedbackComponent->PlayCombatFeedback(
			Context,
			FeedbackData,
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

void UPlayerDefenseComponent::Debug_ReceiveTestAttackFront()
{
	Debug_ReceiveTestAttack(EHitReactionDirection::Front);
}

void UPlayerDefenseComponent::Debug_ReceiveTestAttackBack()
{
	Debug_ReceiveTestAttack(EHitReactionDirection::Back);
}

void UPlayerDefenseComponent::Debug_ReceiveTestAttackLeft()
{
	Debug_ReceiveTestAttack(EHitReactionDirection::Left);
}

void UPlayerDefenseComponent::Debug_ReceiveTestAttackRight()
{
	Debug_ReceiveTestAttack(EHitReactionDirection::Right);
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
		OwnerLocation + HitDirection * 10.0f;

	const EDefenseResult Result =
		ResolveIncomingAttack(Context);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Debug Test Attack Result: %s"),
		*UEnum::GetValueAsString(Result)
	);
}

void UPlayerDefenseComponent::HandleOwnerDead()
{
	if (bDeadHandled)
	{
		return;
	}

	bDeadHandled = true;

	if (!OwnerCharacter || !StateComponent)
	{
		return;
	}

	ClearTransientCombatStatesForDead();

	StateComponent->AddStateTag(CombatTags::State_Hit_Dead);
	StateComponent->AddStateTag(CombatTags::State_Movement_Locked);

	if (AController* Controller = OwnerCharacter->GetController())
	{
		if (APlayerController* PlayerController =
			Cast<APlayerController>(Controller))
		{
			PlayerController->SetIgnoreMoveInput(true);
			PlayerController->SetIgnoreLookInput(true);
		}
	}

	if (UCharacterMovementComponent* MovementComponent =
		OwnerCharacter->GetCharacterMovement())
	{
		MovementComponent->StopMovementImmediately();
		MovementComponent->DisableMovement();
	}

	if (UPlayerLocomotionComponent* LocomotionComponent =
		OwnerCharacter->GetLocomotionComponent())
	{
		LocomotionComponent->DoStopSprint();
		LocomotionComponent->RefreshMovementSettings();
	}

	if (WeaponComponent)
	{
		WeaponComponent->EndWeaponHitCheck();
	}

	PlayDeadMontage();
}

void UPlayerDefenseComponent::ClearTransientCombatStatesForDead()
{
	if (!StateComponent)
	{
		return;
	}

	StateComponent->RemoveStateTag(CombatTags::State_Combat_Attacking);
	StateComponent->RemoveStateTag(CombatTags::State_Combat_Guarding);
	StateComponent->RemoveStateTag(CombatTags::State_Combat_Parry);
	StateComponent->RemoveStateTag(CombatTags::State_Combat_Dodging);
	StateComponent->RemoveStateTag(CombatTags::State_Combat_Invincible);
	StateComponent->RemoveStateTag(CombatTags::State_Hit_Reacting);
	StateComponent->RemoveStateTag(CombatTags::State_Hit_PostureBroken);
}

void UPlayerDefenseComponent::PlayDeadMontage()
{
	UE_LOG(LogTemp, Warning, TEXT("PlayDeadMontage called"));

	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("Dead: OwnerCharacter null"));
		FinalizeDead();
		return;
	}

	USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
	if (!Mesh)
	{
		UE_LOG(LogTemp, Error, TEXT("Dead: Mesh null"));
		FinalizeDead();
		return;
	}

	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Dead: AnimInstance null"));
		FinalizeDead();
		return;
	}

	UAnimMontage* DeadMontage =
		DefenseData ? DefenseData->DeadMontage : nullptr;

	if (!DeadMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("Dead: DeadMontage null"));
		FinalizeDead();
		return;
	}

	Mesh->bPauseAnims = false;

	AnimInstance->StopAllMontages(0.05f);

	const float Duration = AnimInstance->Montage_Play(
		DeadMontage,
		DefenseData ? DefenseData->DeadMontagePlayRate : 1.0f
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Dead Montage Play Duration: %.3f / Montage: %s"),
		Duration,
		*GetNameSafe(DeadMontage)
	);

	if (Duration <= 0.0f)
	{
		UE_LOG(LogTemp, Error, TEXT("Dead: Montage_Play failed"));
		FinalizeDead();
		return;
	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(
		this,
		&UPlayerDefenseComponent::OnDeadMontageEnded
	);

	AnimInstance->Montage_SetEndDelegate(
		EndDelegate,
		DeadMontage
	);
}

void UPlayerDefenseComponent::OnDeadMontageEnded(
	UAnimMontage* Montage,
	bool bInterrupted)
{
	FinalizeDead();
}

void UPlayerDefenseComponent::FinalizeDead()
{
	if (!OwnerCharacter || !StateComponent)
	{
		return;
	}

	StateComponent->AddStateTag(CombatTags::State_Hit_Dead);
	StateComponent->AddStateTag(CombatTags::State_Movement_Locked);

	if (DefenseData && DefenseData->bFreezePoseAfterDead)
	{
		if (USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh())
		{
			Mesh->bPauseAnims = true;
		}
	}
}

void UPlayerDefenseComponent::HandleOwnerPostureBroken()
{
	if (!OwnerCharacter || !StateComponent || !AttributeComponent)
	{
		return;
	}

	if (AttributeComponent->IsDead())
	{
		return;
	}

	ClearCombatStatesForPostureBreak();

	StateComponent->AddStateTag(
		CombatTags::State_Movement_Locked
	);

	if (UCharacterMovementComponent* Movement =
		OwnerCharacter->GetCharacterMovement())
	{
		Movement->StopMovementImmediately();
	}

	PlayPostureBrokenMontage();
}

void UPlayerDefenseComponent::HandleOwnerPostureRecovered()
{
	if (!OwnerCharacter || !StateComponent || !AttributeComponent)
	{
		return;
	}

	if (AttributeComponent->IsDead())
	{
		return;
	}

	StateComponent->RemoveStateTag(
		CombatTags::State_Movement_Locked
	);

	if (UPlayerLocomotionComponent* LocomotionComponent =
		OwnerCharacter->GetLocomotionComponent())
	{
		LocomotionComponent->RefreshMovementSettings();
	}
}

void UPlayerDefenseComponent::PlayPostureBrokenMontage()
{
	if (!OwnerCharacter || !DefenseData)
	{
		return;
	}

	UAnimMontage* Montage = DefenseData->PostureBrokenMontage;

	if (!Montage)
	{
		return;
	}

	USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
	if (!Mesh)
	{
		return;
	}

	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	AnimInstance->StopAllMontages(0.05f);

	const float Duration =
		AnimInstance->Montage_Play(
			Montage,
			DefenseData->PostureBrokenMontagePlayRate
		);

	if (Duration <= 0.0f)
	{
	}
}

void UPlayerDefenseComponent::ClearCombatStatesForPostureBreak()
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

	StateComponent->RemoveStateTag(
		CombatTags::State_Combat_Invincible
	);

	DisableInvincible();

	if (UPlayerAttackComponent* AttackComponent =
		OwnerCharacter->GetAttackComponent())
	{
		AttackComponent->CancelAttackForPostureBreak();
	}

	if (WeaponComponent)
	{
		WeaponComponent->EndWeaponHitCheck();
	}

	if (UPlayerLocomotionComponent* LocomotionComponent =
		OwnerCharacter->GetLocomotionComponent())
	{
		LocomotionComponent->CancelDodgeForPostureBreak();
		LocomotionComponent->RefreshMovementSettings();
	}
}
