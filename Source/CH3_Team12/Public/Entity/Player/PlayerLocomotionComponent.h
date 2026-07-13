#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Framework/DataAsset/PlayerLocomotionDataAsset.h"
#include "Framework/DataAsset/PlayerDodgeDataAsset.h"
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

	// Sprint
	void DoStartSprint();
	void DoStopSprint();

	void Dodge(const FInputActionValue& Value); // 안씀
	void EndDodge();
	void OpenDodgeMove();
	
	void OnDodgeSprintPressed(const FInputActionValue& Value);
	void OnDodgeSprintReleased(const FInputActionValue& Value);
	
	// Getter
	FORCEINLINE float GetNormalWalkSpeed() const { return NormalWalkSpeed; }
	FORCEINLINE float GetSprintSpeed() const { return SprintSpeed; }
	FORCEINLINE FVector2D GetLastMovementInput() const { return LastMovementInput; }

	FVector GetDodgeWorldDirectionFromLastInput() const;

	void RefreshMovementSettings();

private:
	bool CanMove() const;
	bool CanSprint() const;
	bool CanDodge() const;

	void StartDodge();
	void PlayDodgeMontage(const FEvadeMontageData& EvadeData);
	void OnDodgeMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	EDodgeDirection CalculateDodgeDirectionFromInput(
		const FVector2D& InputValue
	) const;
	
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
	
	UPROPERTY(EditDefaultsOnly, Category="Locomotion|Dodge|Data")
	TObjectPtr<UPlayerDodgeDataAsset> DodgeData;
	
	FVector2D LastMovementInput = FVector2D::ZeroVector;

	// Speed
	float NormalWalkSpeed = 0.0f;
	float WalkSpeed = 0.0f;
	float SprintSpeed = 0.0f;
	float LockOnWalkSpeed = 0.0f;
	float GuardWalkSpeed = 0.0f;
	
	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentDodgeMontage;

	bool ShouldUseDirectionalDodge() const;
	
	bool TryStartDodge();
	void TryStartSprintAfterDodge();

	bool bDodgeSprintHeld = false;
	bool bWantsSprintAfterDodge = false;
	
private:
	bool bBufferedDodgeInput = false;
	float BufferedDodgeInputTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category="Dodge|Buffer")
	float DodgeInputBufferDuration = 0.2f;

private:
	void BufferDodgeInput();
	bool HasValidBufferedDodgeInput() const;
	void ClearBufferedDodgeInput();

public:
	bool TryConsumeBufferedDodge();
	
};