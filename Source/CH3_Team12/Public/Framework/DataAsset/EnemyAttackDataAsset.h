// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Combat/CombatTypes.h"
#include "EnemyAttackDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FEnemyAttackMontageSet
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack | Montage")
	TObjectPtr<UAnimMontage> AttackMontage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack | Montage")
	TObjectPtr<UAnimMontage> ParriedMontage;
};

USTRUCT(BlueprintType)
struct FAttackAnimationData
{
	GENERATED_BODY()
public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack|Animation")
	FEnemyAttackMontageSet AttackMontageSet;
};

UCLASS()
class CH3_TEAM12_API UEnemyAttackDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	FAttackAnimationData AttackAnimationData;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack", meta = (ClampMin = 1.0f, UIMin = 1.0f))
	float AttackRange = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack", meta = (ClampMin = 0.1f, UIMin = 0.1f, ClampMax = 10.0f, UIMax = 10.0f))
	float AttackCooldown = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack", meta = (ClampMin = 0.1f, UIMin = 0.1f))
	float DamageRate = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack", meta = (ClampMin = 0.1f, UIMin = 0.1f))
	float PostureDamageRate = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack", meta = (ClampMin = 0.1f, UIMin = 0.1f))
	float Rebound = 10.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack")
	FAttackInfo AttackInfo;
};
