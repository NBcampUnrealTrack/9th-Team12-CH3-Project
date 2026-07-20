// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyAttributeDataAsset.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class CH3_TEAM12_API UEnemyAttributeDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Health")
	float MaxHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Posture")
	float MaxPosture = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Posture")
	float PostureRecoveryRate = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Posture")
	float PostureRecoveryDelay = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Posture")
	float PostureBreakDuration = 1.5f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Posture")
	float MaxInnerPosture = 50.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Posture")
	float InnerPostureRecoveryRate = 5.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Posture")
	float InnerPostureRecoveryDelay = 2.0f;
};
