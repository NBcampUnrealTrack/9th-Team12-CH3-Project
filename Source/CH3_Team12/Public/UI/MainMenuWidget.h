#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuWidget.generated.h"

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnButtonClicked);

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
