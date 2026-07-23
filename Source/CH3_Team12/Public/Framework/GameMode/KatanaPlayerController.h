#pragma once

#include "CoreMinimal.h"
#include "Entity/Player/PlayerControllerBase.h"
#include "KatanaPlayerController.generated.h"

class AEnemyCharacterBase;
class APlayerCharacterBase;
struct FInputActionValue;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API AKatanaPlayerController : public APlayerControllerBase
{
	GENERATED_BODY()

public:
	APlayerCharacterBase* GetPlayerCharacter() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UInputMappingContext> InputMappingContextUI = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UInputAction> InGameMenuActon = nullptr;

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;
	virtual void OnUnPossess() override;

private:
	UPROPERTY()
	TWeakObjectPtr<APlayerCharacterBase> PlayerCharacterBase;

	UPROPERTY()
	TWeakObjectPtr<AEnemyCharacterBase> EnemyCharacterBase;

	AEnemyCharacterBase* FindEnemyCharacter() const;
	void ToggleInGameMenu(const FInputActionValue& Value);

	UFUNCTION()
	void HandleBossRoomEntered();

	UFUNCTION()
	void HandleEnemyDeath();

	UFUNCTION()
	void HandleEnemyTransitionFinished();

	UFUNCTION()
	void HandlePlayerDead();

	UFUNCTION()
	void HandlePlayerLockOnStateChanged(bool bIsLockOn, AActor* LockOnTarget);

	FTimerHandle DelayStageResultTimerHandle;
	FTimerHandle DelayPlayerDeathTimerHandle;

	UFUNCTION()
	void HandleDelayStageResult();

	UFUNCTION()
	void HandleDelayPlayerDeath();
};
