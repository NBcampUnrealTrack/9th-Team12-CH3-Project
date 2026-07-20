#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCameraComponent.generated.h"

struct FInputActionValue;

class APlayerCharacterBase;
class USpringArmComponent;
class UCameraComponent;
class UStateTagComponent;
class UPlayerCameraDataAsset;
class AActor;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnLockOnStateChanged,
	bool,
	bIsLockOn,
	AActor*,
	LockOnTarget
);

UENUM(BlueprintType)
enum class EPlayerCameraMode : uint8
{
	Normal,
	LockOn,
	Execution
};

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
	bool IsLockOn() const;
	
	void StartExecutionCamera(AActor* ExecutionTarget);
	void EndExecutionCamera();
	
	UPROPERTY(BlueprintAssignable, Category="Camera|LockOn")
	FOnLockOnStateChanged OnLockOnStateChanged;
	
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
	
	float GetActorHalfHeight(AActor* Actor) const;
	void UpdateExecutionCamera(float DeltaTime);
	void ApplyExecutionCamera(float DeltaTime, bool bInstant);
	
	void ApplyNormalCameraInstant();
	void StartNormalCameraTransition();

	AActor* FindLockOnTarget() const;

	FVector GetLockOnFocusLocation(
		AActor* Actor,
		float HeightRatio
	) const;
	void ApplyCameraCollisionSettings();
	void PrepareNormalCameraTransitionFromExecution();

	UPROPERTY()
	TObjectPtr<AActor> CurrentExecutionTarget;
	
	EPlayerCameraMode CameraMode = EPlayerCameraMode::Normal;

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

	UPROPERTY(EditDefaultsOnly, Category="Camera|Data")
	TObjectPtr<UPlayerCameraDataAsset> CameraData;
};