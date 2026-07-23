#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/UICommonTypes.h"
#include "MainMenuSettingsWidget.generated.h"

class UInputSettingsWidget;
class UGraphicSettingsWidget;
class UButton;
class USoundSettingsWidget;

/**
 *
 */
UCLASS()
class CH3_TEAM12_API UMainMenuSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnButtonClicked OnBtnResetStageRecordClicked;
	FOnButtonClicked OnBtnBackClicked;

	USoundSettingsWidget* GetSoundSettings() const;
	UGraphicSettingsWidget* GetGraphicSettings() const;
	UInputSettingsWidget* GetInputSettings() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> ClickSound;

	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnBack;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnSoundSettings;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnGraphicSettings;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnInputSettings;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnResetStageRecord;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USoundSettingsWidget> SoundSettingsWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UGraphicSettingsWidget> GraphicSettingsWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInputSettingsWidget> InputSettingsWidget;

	UFUNCTION()
	void HandleBtnBackClicked();

	UFUNCTION()
	void HandleBtnSoundSettingsClicked();

	UFUNCTION()
	void HandleBtnGraphicSettingsClicked();

	UFUNCTION()
	void HandleBtnInputSettingsClicked();

	UFUNCTION()
	void HandleBtnResetStageRecordClicked();

	UFUNCTION()
	void HandleVisibilitySoundSettingsChanged(ESlateVisibility InVisibility);

	UFUNCTION()
	void HandleVisibilityGraphicSettingsChanged(ESlateVisibility InVisibility);

	UFUNCTION()
	void HandleVisibilityInputSettingsChanged(ESlateVisibility InVisibility);
};
