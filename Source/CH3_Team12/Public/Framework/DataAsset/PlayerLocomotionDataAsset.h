#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerLocomotionDataAsset.generated.h"

UCLASS(BlueprintType)
class CH3_TEAM12_API UPlayerLocomotionDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Speed")
	float NormalWalkSpeed = 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Speed")
	float WalkSpeed = 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Speed")
	float SprintSpeed = 600.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Speed")
	float LockOnWalkSpeed = 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Speed")
	float GuardWalkSpeed = 400.0f;
};