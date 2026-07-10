#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerAttributeDataAsset.generated.h"

UCLASS(BlueprintType)
class CH3_TEAM12_API UPlayerAttributeDataAsset : public UDataAsset
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
};