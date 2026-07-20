#pragma once

#include "CoreMinimal.h"
#include "UICommonTypes.h"
#include "Blueprint/UserWidget.h"
#include "PlayerDeathWidget.generated.h"

class UButton;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UPlayerDeathWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnButtonClicked OnRestartButtonClicked;
	FOnButtonClicked OnMainMenuButtonClicked;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnRestart;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnMainMenu;

	UFUNCTION()
	void HandleRestartButtonClicked();

	UFUNCTION()
	void HandleMainMenuButtonClicked();
};
