#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerCombatMontageDataAsset.generated.h"

class UAnimMontage;

UCLASS(BlueprintType)
class CH3_TEAM12_API UPlayerCombatMontageDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	TObjectPtr<UAnimMontage> LightAttackMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attack")
	TObjectPtr<UAnimMontage> HeavyAttackMontage;

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
	TObjectPtr<UAnimMontage> HitLeftMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hit")
	TObjectPtr<UAnimMontage> HitRightMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Hit")
	TObjectPtr<UAnimMontage> HitBackMontage;
};