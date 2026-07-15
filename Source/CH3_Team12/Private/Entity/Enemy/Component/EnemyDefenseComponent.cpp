// Fill out your copyright notice in the Description page of Project Settings.

#include "Entity/Enemy/Component/EnemyDefenseComponent.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Entity/Enemy/EnemyCharacterBase.h"
#include "Entity/Enemy/Component/EnemyAttackComponent.h"
#include "Entity/Enemy/Component/EnemyAttributeComponent.h"
#include "Entity/Player/StateTagComponent.h"
#include "GameplayTags/CombatGameplayTags.h"

UEnemyDefenseComponent::UEnemyDefenseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEnemyDefenseComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AEnemyCharacterBase>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyDefenseComponent : OwnerCharacter is nullptr"));
		return;
	}

	StateComponent = OwnerCharacter->GetStateTagComponent();
	AttributeComponent = OwnerCharacter->GetEnemyAttributeComponent();
	AttackComponent = OwnerCharacter->GetEnemyAttackComponent();

	if (!StateComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyDefenseComponent : StateComponent is nullptr"));
	}

	if (!AttributeComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyDefenseComponent : AttributeComponent is nullptr"));
	}
}

EDefenseResult UEnemyDefenseComponent::ResolveIncomingAttack(
	const FIncomingAttackContext& Context)
{
	if (!OwnerCharacter || !StateComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy ResolveIncomingAttack: None"));
		return EDefenseResult::None;
	}

	if (AttributeComponent && AttributeComponent->IsDead())
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy ResolveIncomingAttack: Dead"));
		return EDefenseResult::None;
	}

	if (StateComponent->HasStateTagExact(
		CombatTags::State_Combat_Invincible))
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy ResolveIncomingAttack: Invincible"));
		return EDefenseResult::Invincible;
	}

	const EHitReactionDirection ReactionDirection =
		CalculateHitReactionDirection(Context);

	if (IsParrying() && Context.AttackInfo.bCanBeParried)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Enemy ResolveIncomingAttack: Parry / Direction: %s"),
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
			TEXT("Enemy ResolveIncomingAttack: Guard / Direction: %s"),
			*UEnum::GetValueAsString(ReactionDirection)
		);

		HandleGuardSuccess(Context, ReactionDirection);
		return EDefenseResult::Guard;
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Enemy ResolveIncomingAttack: Hit / Direction: %s"),
		*UEnum::GetValueAsString(ReactionDirection)
	);

	HandleDirectHit(Context, ReactionDirection);
	return EDefenseResult::Hit;
}

bool UEnemyDefenseComponent::IsGuarding() const
{
	return StateComponent &&
		StateComponent->HasStateTagExact(
			CombatTags::State_Combat_Guarding
		);
}

bool UEnemyDefenseComponent::IsParrying() const
{
	return StateComponent &&
		StateComponent->HasStateTagExact(
			CombatTags::State_Combat_Parry
		);
}

bool UEnemyDefenseComponent::CanHitReaction() const 
{
	bool Result = true;
	FGameplayTagContainer UnAllowState;
	UnAllowState.AddTag(CombatTags::State_Hit_Dead);
	UnAllowState.AddTag(CombatTags::State_Combat_Attacking);
	UnAllowState.AddTag(CombatTags::State_Hit_PostureBroken);
	UnAllowState.AddTag(CombatTags::State_Action_Executing);
	Result = !(StateComponent->HasAnyStateTags(UnAllowState));

	return Result;
}

EHitReactionDirection UEnemyDefenseComponent::CalculateHitReactionDirection(
	const FIncomingAttackContext& Context) const
{
	if (!OwnerCharacter)
	{
		return EHitReactionDirection::Front;
	}

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

void UEnemyDefenseComponent::HandleParrySuccess(
	const FIncomingAttackContext& Context,
	EHitReactionDirection ReactionDirection)
{
	PlayMontageSafe(ParryReactionMontage);
}

void UEnemyDefenseComponent::HandleGuardSuccess(
	const FIncomingAttackContext& Context,
	EHitReactionDirection ReactionDirection)
{
	PlayMontageSafe(GuardHitMontage);

	if (AttributeComponent)
	{
		AttributeComponent->ApplyAttributeDamage(
			Context.AttackInfo.Damage * GuardChipDamageRate,
			Context.AttackInfo.PostureDamage * GuardPostureDamageRate
		);
	}
}

void UEnemyDefenseComponent::HandleDirectHit(
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

	//StateComponent->RemoveStateTag(CombatTags::State_Combat_Attacking);
	//StateComponent->RemoveStateTag(CombatTags::State_Combat_Guarding);
	//StateComponent->RemoveStateTag(CombatTags::State_Combat_Parry);
	
	// if (AttackComponent)
	// {
	// 	AttackComponent->CancelAttack();
	// }

	if (AttributeComponent)
	{
		AttributeComponent->ApplyAttributeDamage(
			Context.AttackInfo.Damage,
			Context.AttackInfo.PostureDamage
		);

		if (AttributeComponent->IsDead())
		{
			return;
		}
	}

	if (CanHitReaction())
	{
		if (!PlayMontageSafe(HitReactionMontage))
		{
			EndHitReaction();
		}
	}
}

bool UEnemyDefenseComponent::PlayMontageSafe(
	UAnimMontage* Montage,
	float PlayRate) const
{
	if (!OwnerCharacter || !Montage)
	{
		return false;
	}

	return OwnerCharacter->PlayAnimMontage(
		Montage,
		PlayRate
	) > 0.0f;
}

void UEnemyDefenseComponent::OnHitReactionMontageEnded(
	UAnimMontage* Montage,
	bool bInterrupted)
{
	EndHitReaction();
}

void UEnemyDefenseComponent::EndHitReaction()
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
}
