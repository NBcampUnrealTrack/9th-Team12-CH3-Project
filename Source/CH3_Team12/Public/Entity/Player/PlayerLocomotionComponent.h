#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerLocomotionComponent.generated.h"

class UAnimMontage;
class APlayerCharacterBase;
class UStateTagComponent;
class UCharacterMovementComponent;
class UPlayerCombatComponent;
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
	bool CanSprint() const;
	bool CanDodge() const;

	void TryStartSprintByHold();
	void RequestDodge();
	void StartDodge(const FVector& DodgeDirection);
	void ClearDodgeBuffer();

private:
	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UStateTagComponent> StateComponent;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	UPROPERTY()
	TObjectPtr<UPlayerCombatComponent> CombatComponent;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Locomotion|Speeds", meta=(AllowPrivateAccess="true"))
	float NormalWalkSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Locomotion|Speeds", meta=(AllowPrivateAccess="true"))
	float SprintSpeed = 400.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Locomotion|Speeds", meta=(AllowPrivateAccess="true"))
	float LockOnWalkSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Locomotion|Speeds", meta=(AllowPrivateAccess="true"))
	float GuardWalkSpeed = 200.0f;
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Locomotion|Dodge", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAnimMontage> DodgeMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Locomotion|Dodge", meta=(AllowPrivateAccess="true"))
	float SprintHoldThreshold = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Locomotion|Dodge", meta=(AllowPrivateAccess="true"))
	float DodgeBufferDuration = 0.15f;

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

	UPROPERTY(EditDefaultsOnly, Category="Locomotion|Dodge")
	float DodgeBlendOutTime = 0.12f;

};