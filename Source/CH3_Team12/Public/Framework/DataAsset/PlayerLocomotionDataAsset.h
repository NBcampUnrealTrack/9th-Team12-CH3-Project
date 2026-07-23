#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerLocomotionDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FMontageData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName SectionName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PlayRate = 1.0f;

	bool IsValid() const
	{
		return Montage != nullptr;
	}
};

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
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Speed")
	float JumpZVelocity = 800.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Speed")
	float GravityScale = 2.4f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Speed")
	float AirControl = 0.35f;
};