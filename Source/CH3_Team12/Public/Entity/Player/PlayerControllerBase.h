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
	TObjectPtr<UInputMappingContext> InputMappingContext;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> SprintDodgeAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LockOnAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> DodgeAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> GuardAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> TestAction1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> TestAction2;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> TestAction3;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> TestAction4;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> EquipAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> UseItemAction;
	
	virtual void BeginPlay() override;
	
public:
	// getter
	FORCEINLINE TObjectPtr<UInputMappingContext> GetInputMappingContext() const { return InputMappingContext; }
	FORCEINLINE TObjectPtr<UInputAction> GetMoveAction() const { return MoveAction; }
	FORCEINLINE TObjectPtr<UInputAction> GetJumpAction() const { return JumpAction; }
	FORCEINLINE TObjectPtr<UInputAction> GetLookAction() const { return LookAction; }
	FORCEINLINE TObjectPtr<UInputAction> GetSprintDodgeAction() const { return SprintDodgeAction; }
	FORCEINLINE TObjectPtr<UInputAction> GetAttackAction() const { return AttackAction; }
	FORCEINLINE TObjectPtr<UInputAction> GetLockOnAction() const { return LockOnAction; }
	FORCEINLINE TObjectPtr<UInputAction> GetDodgeAction() const { return DodgeAction; }
	FORCEINLINE TObjectPtr<UInputAction> GetGuardAction() const { return GuardAction; }
	
	FORCEINLINE TObjectPtr<UInputAction> GetTestAction1() const { return TestAction1; }
	FORCEINLINE TObjectPtr<UInputAction> GetTestAction2() const { return TestAction2; }
	FORCEINLINE TObjectPtr<UInputAction> GetTestAction3() const { return TestAction3; }
	FORCEINLINE TObjectPtr<UInputAction> GetTestAction4() const { return TestAction4; }
	
	FORCEINLINE TObjectPtr<UInputAction> GetEquipAction() const { return EquipAction; }
	FORCEINLINE TObjectPtr<UInputAction> GetUseItemAction() const { return UseItemAction; }
};
