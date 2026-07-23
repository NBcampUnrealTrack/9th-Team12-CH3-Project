// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/CombatTypes.h"
#include "EnemyDefenseComponent.generated.h"

class AEnemyCharacterBase;
class UAnimMontage;
class UEnemyAttackComponent;
class UEnemyAttributeComponent;
class UStateTagComponent;

USTRUCT(BlueprintType)
struct FHitSoundData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sound | Hit")
	TObjectPtr<USoundBase> HitSound = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sound | Hit")
	float VolumeMultiplier = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sound | Hit")
	float PitchMultiplier = 1.0f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CH3_TEAM12_API UEnemyDefenseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyDefenseComponent();

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category="Enemy|Defense")
	EDefenseResult ResolveIncomingAttack(const FIncomingAttackContext& Context);

	UFUNCTION(BlueprintPure, Category="Enemy|Defense")
	EHitReactionDirection CalculateHitReactionDirection(
		const FIncomingAttackContext& Context
	) const;

private:
	bool IsGuarding() const;
	bool IsParrying() const;
	bool CanHitReaction() const;

	void HandleParrySuccess(
		const FIncomingAttackContext& Context,
		EHitReactionDirection ReactionDirection
	);

	void HandleGuardSuccess(
		const FIncomingAttackContext& Context,
		EHitReactionDirection ReactionDirection
	);

	void HandleDirectHit(
		const FIncomingAttackContext& Context,
		EHitReactionDirection ReactionDirection
	);

	bool PlayMontageSafe(
		UAnimMontage* Montage,
		float PlayRate = 1.0f
	) const;

	void OnHitReactionMontageEnded(
		UAnimMontage* Montage,
		bool bInterrupted
	);

	void EndHitReaction();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Sound | Hit")
	FHitSoundData HitSoundData;
	
private:
	UPROPERTY()
	TObjectPtr<AEnemyCharacterBase> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UStateTagComponent> StateComponent;

	UPROPERTY()
	TObjectPtr<UEnemyAttributeComponent> AttributeComponent;

	UPROPERTY()
	TObjectPtr<UEnemyAttackComponent> AttackComponent;

	UPROPERTY(EditDefaultsOnly, Category="Enemy|Defense")
	TObjectPtr<UAnimMontage> HitReactionMontage;

	UPROPERTY(EditDefaultsOnly, Category="Enemy|Defense")
	TObjectPtr<UAnimMontage> GuardHitMontage;

	UPROPERTY(EditDefaultsOnly, Category="Enemy|Defense")
	TObjectPtr<UAnimMontage> ParryReactionMontage;

	UPROPERTY(EditDefaultsOnly, Category="Enemy|Defense", meta=(ClampMin=0.0, UIMin=0.0))
	float GuardChipDamageRate = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category="Enemy|Defense", meta=(ClampMin=0.0, UIMin=0.0))
	float GuardPostureDamageRate = 1.0f;
};
