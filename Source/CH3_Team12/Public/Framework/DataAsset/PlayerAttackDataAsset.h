// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerAttackDataAsset.generated.h"

class UAnimMontage;

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	Light,
	Heavy
};

UCLASS(BlueprintType)
class CH3_TEAM12_API UPlayerAttackDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	TObjectPtr<UAnimMontage> LightAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	TObjectPtr<UAnimMontage> HeavyAttackMontage;
	
	UPROPERTY(EditDefaultsOnly, Category="Attack")
	TArray<FName> ComboSectionNames = {
		TEXT("Attack0"),
		TEXT("Attack1"),
		TEXT("Attack2"),
	};

	UPROPERTY(EditDefaultsOnly, Category="Attack")
	float AttackPlayRate = 1.0f;
};
