#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerBase.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class CH3_TEAM12_API APlayerControllerBase : public APlayerController
{
	GENERATED_BODY()
	
public:
	APlayerControllerBase();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* SprintAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* AttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LockOnAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* DodgeAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* ParryingAction;
	
	virtual void BeginPlay() override;
	
public:
	// getter
	FORCEINLINE UInputMappingContext* GetInputMappingContext() const { return InputMappingContext; }
	FORCEINLINE UInputAction* GetMoveAction() const { return MoveAction; }
	FORCEINLINE UInputAction* GetJumpAction() const { return JumpAction; }
	FORCEINLINE UInputAction* GetLookAction() const { return LookAction; }
	FORCEINLINE UInputAction* GetSprintAction() const { return SprintAction; }
	FORCEINLINE UInputAction* GetAttackAction() const { return AttackAction; }
	FORCEINLINE UInputAction* GetLockOnAction() const { return LockOnAction; }
	FORCEINLINE UInputAction* GetDodgeAction() const { return DodgeAction; }
	FORCEINLINE UInputAction* GetParryingAction() const { return ParryingAction; }
};
