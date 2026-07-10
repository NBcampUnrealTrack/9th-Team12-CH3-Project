#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerCameraDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FNormalCameraSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TargetArmLength = 350.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector CameraBoomRelativeLocation = FVector(0.0f, 0.0f, 50.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FRotator CameraBoomRelativeRotation = FRotator(-10.0f, 0.0f, 0.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector SocketOffset = FVector(0.0f, 0.0f, 40.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector TargetOffset = FVector::ZeroVector;
};

USTRUCT(BlueprintType)
struct FLockOnTraceSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TraceRadius = 1500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName EnemyTagName = TEXT("Enemy");
};

USTRUCT(BlueprintType)
struct FLockOnCameraSettings
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BreakDistance = 1500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float NearDistance = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FarDistance = 1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CloseArmLength = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FarArmLength = 700.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ClosePivotHeight = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FarPivotHeight = 50.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RotationInterpSpeed = 7.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CameraInterpSpeed = 6.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CloseFocusBias = 0.45f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FarFocusBias = 0.60f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PlayerFocusHeightRatio = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float NormalTargetFocusHeightRatio = 0.35f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float LargeTargetFocusHeightRatio = 0.10f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float LargeTargetThreshold = 1.4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HeightDifferencePivotScale = 0.15f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinHeightAdjustment = -20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHeightAdjustment = 60.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinPitch = -45.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxPitch = 5.0f;
};

UCLASS(BlueprintType)
class CH3_TEAM12_API UPlayerCameraDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera|Normal")
	FNormalCameraSettings Normal;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera|LockOn|Trace")
	FLockOnTraceSettings Trace;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera|LockOn")
	FLockOnCameraSettings LockOn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera|Interp")
	float NormalCameraInterpSpeed = 6.0f;
};