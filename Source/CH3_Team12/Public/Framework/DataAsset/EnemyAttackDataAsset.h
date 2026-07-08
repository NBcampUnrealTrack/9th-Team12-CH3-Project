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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HitBox")
	FName ActiveHitSocket;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HitBox", meta = (ClampMin = 0, UIMin = 0))
	float TraceRadius = 150.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HitBox", meta = (ClampMax = 50, UIMax = 50))
	int32 TraceSampleCount = 5;
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
};
