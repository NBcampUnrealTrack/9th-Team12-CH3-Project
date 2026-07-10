#pragma once

#include "CoreMinimal.h"
#include "Entity/Player/PlayerControllerBase.h"
#include "KatanaPlayerController.generated.h"

struct FInputActionValue;
class UPlayerAttributeComponent;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API AKatanaPlayerController : public APlayerControllerBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UInputMappingContext> InputMappingContextUI = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UInputAction> InGameMenuActon = nullptr;

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void SetupInputComponent() override;

private:
	void ToggleInGameMenu(const FInputActionValue& Value);
};
