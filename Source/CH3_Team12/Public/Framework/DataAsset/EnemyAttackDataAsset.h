// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyAttackDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FHitBoxData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack|Socket")
	FName ActiveHitSocket;
};

USTRUCT(BlueprintType)
struct FAttackAnimationData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack", meta = (ClampMin = 1.0f, UIMin = 1.0f))
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack", meta = (ClampMin = 0.1f, UIMin = 0.1f, ClampMax = 10.0f, UIMax = 10.0f))
	float AttackCooldown;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack | Montage")
	TObjectPtr<UAnimMontage> Montage;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attack|Hitbox")
	TArray<FHitBoxData> HitBoxes;
};

/**
 * 
 */
UCLASS()
class CH3_TEAM12_API UEnemyAttackDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	FORCEINLINE const FAttackAnimationData* GetAttackAnimationData(int32 InIndex) const
	{
		if (AttackAnimationDatas.IsValidIndex(InIndex))
		{
			return &AttackAnimationDatas[InIndex];
		}
		
		UE_LOG(LogTemp, Error, TEXT("Katana_UEnemyAttackDataAsset : Invalid index."));
		return nullptr;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	TArray<FAttackAnimationData> AttackAnimationDatas;
	
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
	// float AttackRange = 250.f;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
	// float AttackCooldown = 2.f;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack | Montage")
	// TObjectPtr<UAnimMontage> NormalAttack0;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack | Montage")
	// TObjectPtr<UAnimMontage> NormalAttack1;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack | Montage")
	// TObjectPtr<UAnimMontage> StrongAttack;
	//
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack | Montage")
	// TObjectPtr<UAnimMontage> FarStrongAttack;
};
