#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Framework/DataAsset/PlayerLocomotionDataAsset.h"
#include "PlayerLocomotionComponent.generated.h"

class UAnimMontage;
class APlayerCharacterBase;
class UStateTagComponent;
class UCharacterMovementComponent;
class UPlayerAttackComponent;
class UPlayerDefenseComponent;

struct FInputActionValue;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CH3_TEAM12_API UPlayerLocomotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerLocomotionComponent();

protected:
	virtual void BeginPlay() override;

public:
	
	// Input
	void DoStartJump(const FInputActionValue& Value);
	void DoStopJump(const FInputActionValue& Value);

	void DoMove(const FInputActionValue& Value);
	void DoStopMove();

	// Sprint / Dodge shared input
	void OnSprintDodgePressed(const FInputActionValue& Value);
	void OnSprintDodgeReleased(const FInputActionValue& Value);

	// Sprint
	void DoStartSprint();
	void DoStopSprint();

	// Dodge notify / buffer
	void OpenDodgeBufferWindow();
	void CloseDodgeBufferWindow();
	void ConsumeDodgeBuffer();
	void EndDodge();

	// Getter
	FORCEINLINE float GetNormalWalkSpeed() const { return NormalWalkSpeed; }
	FORCEINLINE float GetSprintSpeed() const { return SprintSpeed; }
	FORCEINLINE FVector2D GetLastMovementInput() const { return LastMovementInput; }

	FVector GetDodgeWorldDirectionFromLastInput() const;

	void OpenDodgeRecovery();
	void RefreshMovementSettings();

private:
	bool CanMove() const;
	bool CanSprint() const;
	bool CanDodge() const;

	void TryStartSprintByHold();
	void RequestDodge();
	void StartDodge(const FVector& DodgeDirection);
	void ClearDodgeBuffer();

	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UStateTagComponent> StateComponent;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	UPROPERTY()
	TObjectPtr<UPlayerAttackComponent> AttackComponent;
	
	UPROPERTY()
	TObjectPtr<UPlayerDefenseComponent> DefenseComponent;

	UPROPERTY(EditDefaultsOnly, Category="Locomotion|Data")
	TObjectPtr<UPlayerLocomotionDataAsset> LocomotionData;
	
private:
	FVector2D LastMovementInput = FVector2D::ZeroVector;

	bool bSprintDodgeHeld = false;
	bool bSprintStartedByHold = false;
	float SprintDodgePressedTime = 0.0f;

	FTimerHandle SprintHoldTimerHandle;

	bool bDodgeBufferWindowOpen = false;
	bool bDodgeBuffered = false;
	FVector BufferedDodgeDirection = FVector::ZeroVector;

	FTimerHandle DodgeBufferTimerHandle;
	FTimerHandle DodgeEndTimerHandle;

	// Speed
	float NormalWalkSpeed = 0.0f;
	float WalkSpeed = 0.0f;
	float SprintSpeed = 0.0f;
	float LockOnWalkSpeed = 0.0f;
	float GuardWalkSpeed = 0.0f;
	
	// Dodge
	float SprintHoldThreshold = 0.0f;
	float DodgeBufferDuration = 0.0f;
	float DodgeBlendOutTime = 0.0f;
};