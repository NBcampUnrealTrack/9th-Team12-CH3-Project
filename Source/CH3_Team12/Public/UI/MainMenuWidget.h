#pragma once

#include "CoreMinimal.h"
#include "UICommonTypes.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;

/**
 *
 */
UCLASS()
class CH3_TEAM12_API UMainMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnButtonClicked OnPlayButtonClicked;
	FOnButtonClicked OnSettingsButtonClicked;
	FOnButtonClicked OnQuitButtonClicked;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> ClickSound;

	virtual void NativeConstruct() override;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> PlayButton;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> SettingsButton;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> QuitButton;

	UFUNCTION()
	void HandlePlayButtonClicked() const;

	UFUNCTION()
	void HandleSettingsButtonClicked() const;

	UFUNCTION()
	void HandleQuitButtonClicked() const;
};
