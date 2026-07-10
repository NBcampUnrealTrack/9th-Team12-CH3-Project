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

	UPROPERTY(EditDefaultsOnly, Category="Camera|Data")
	TObjectPtr<UPlayerCameraDataAsset> CameraData;
};