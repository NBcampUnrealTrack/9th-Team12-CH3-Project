#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCameraComponent.generated.h"

struct FInputActionValue;

class APlayerCharacterBase;
class USpringArmComponent;
class UCameraComponent;
class UStateTagComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CH3_TEAM12_API UPlayerCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerCameraComponent();

	virtual void BeginPlay() override;

	void Look(const FInputActionValue& Value);
	void LockOn();

	AActor* GetCurrentLockOnTarget() const { return CurrentLockOnTarget; }
	bool IsLockOnMode() const;

protected:
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

private:
	void TryLockOn();
	void ClearLockOn();
	void SetupLockOnCamera();

	void UpdateLockOnCamera(float DeltaTime);
	void UpdateNormalCamera(float DeltaTime);

	void ApplyNormalCameraInstant();
	void StartNormalCameraTransition();

	AActor* FindLockOnTarget() const;

	FVector GetLockOnFocusLocation(
		AActor* Actor,
		float HeightRatio
	) const;

	float GetActorHalfHeight(AActor* Actor) const;

private:
	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerActor;

	UPROPERTY()
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY()
	TObjectPtr<UCameraComponent> FollowCamera;

	UPROPERTY()
	TObjectPtr<AActor> CurrentLockOnTarget;

	UPROPERTY()
	TObjectPtr<UStateTagComponent> StateTagComponent;

private:
	UPROPERTY(EditAnywhere, Category="Camera|Normal")
	float NormalTargetArmLength = 350.0f;

	UPROPERTY(EditAnywhere, Category="Camera|Normal")
	FVector NormalCameraBoomRelativeLocation = FVector(0.0f, 0.0f, 50.0f);

	UPROPERTY(EditAnywhere, Category="Camera|Normal")
	FRotator NormalCameraBoomRelativeRotation = FRotator(-10.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category="Camera|Normal")
	FVector NormalSocketOffset = FVector(0.0f, 0.0f, 40.0f);

	UPROPERTY(EditAnywhere, Category="Camera|Normal")
	FVector NormalTargetOffset = FVector::ZeroVector;

private:
	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Trace")
	float LockOnTraceRadius = 1500.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Trace")
	FName EnemyTagName = TEXT("Enemy");

private:
	UPROPERTY(EditAnywhere, Category="Camera|Interp")
	float CameraInterpSpeed = 6.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn")
	float LockOnBreakDistance = 1500.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn")
	float LockOnNearDistance = 100.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn")
	float LockOnFarDistance = 1000.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn")
	float LockOnCloseArmLength = 500.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn")
	float LockOnFarArmLength = 700.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn")
	float LockOnClosePivotHeight = 90.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn")
	float LockOnFarPivotHeight = 50.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn")
	float LockOnRotationInterpSpeed = 7.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn")
	float LockOnCameraInterpSpeed = 6.0f;

private:
	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Focus")
	float LockOnCloseFocusBias = 0.45f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Focus")
	float LockOnFarFocusBias = 0.60f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Focus")
	float PlayerFocusHeightRatio = 0.25f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Focus")
	float NormalTargetFocusHeightRatio = 0.35f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Focus")
	float LargeTargetFocusHeightRatio = 0.10f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Focus")
	float LargeTargetThreshold = 1.4f;

private:
	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Height")
	float LockOnHeightDifferencePivotScale = 0.15f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Height")
	float LockOnMinHeightAdjustment = -20.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Height")
	float LockOnMaxHeightAdjustment = 60.0f;

private:
	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Rotation")
	float MinLockOnPitch = -45.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Rotation")
	float MaxLockOnPitch = 5.0f;
};