// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Combat/CombatTypes.h"
#include "PlayerDefenseDataAsset.generated.h"

class UAnimMontage;

UCLASS(BlueprintType)
class CH3_TEAM12_API UPlayerDefenseDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Guard")
	TObjectPtr<UAnimMontage> GuardStartMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Parry") 
	TObjectPtr<UAnimMontage> ParryLeftMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Parry")
	TObjectPtr<UAnimMontage> ParryRightMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Guard")
	TObjectPtr<UAnimMontage> GuardHitLeftMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Guard")
	TObjectPtr<UAnimMontage> GuardHitRightMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hit")
	TObjectPtr<UAnimMontage> HitFrontMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hit")
	TObjectPtr<UAnimMontage> HitBackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hit")
	TObjectPtr<UAnimMontage> HitLeftMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hit")
	TObjectPtr<UAnimMontage> HitRightMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dead")
	TObjectPtr<UAnimMontage> DeadMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="Guard")
	float GuardMontageBlendOutTime = 0.1f;

	UPROPERTY(EditAnywhere, Category="Guard")
	float GuardChipDamageRate = 0.2f;
	
	UPROPERTY(EditAnywhere, Category="Guard")
	float GuardPostureDamageRate = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dead")
	float DeadMontagePlayRate = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Dead")
	bool bFreezePoseAfterDead = true;
	
	UAnimMontage* GetParryReactionMontage(
		EHitReactionDirection ReactionDirection
	) const;

	UAnimMontage* GetGuardHitMontage(
		EHitReactionDirection ReactionDirection
	) const;

	UAnimMontage* GetHitReactionMontage(
		EHitReactionDirection ReactionDirection
	) const;
};
