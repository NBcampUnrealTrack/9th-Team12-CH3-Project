#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerLocomotionComponent.generated.h"

class ACharacter;
class UAnimMontage;
class APlayerCharacterBase;
class UStateTagComponent;
class UCharacterMovementComponent;
struct FInputActionValue;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH3_TEAM12_API UPlayerLocomotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerLocomotionComponent();
	
	virtual void BeginPlay() override;
	
public:
	void DoStartJump(const FInputActionValue& value);
	void DoStopJump(const FInputActionValue& value);
	void DoStartSprint(const FInputActionValue& value);
	void DoStopSprint(const FInputActionValue& value);
	void DoMove(const FInputActionValue& value);
	void Look(const FInputActionValue& value);
	
	// getter
	FORCEINLINE float GetNormalWalkSpeed() const { return NormalWalkSpeed; }
	FORCEINLINE float GetSprintSpeed() const { return SprintSpeed; }
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion|Speeds")
	float NormalWalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion|Speeds")
	float SprintSpeed;
	
private:
	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;
	UPROPERTY()
	TObjectPtr<UStateTagComponent> StateComponent;
	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent;
	
public:
	FVector2D GetLastMovementInput() const { return LastMovementInput; }

	FVector GetDodgeWorldDirectionFromLastInput() const;

	void DoStopMove();

private:
	FVector2D LastMovementInput = FVector2D::ZeroVector;
};
