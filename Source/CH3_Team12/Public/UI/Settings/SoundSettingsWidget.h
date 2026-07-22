#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/UICommonTypes.h"
#include "SoundSettingsWidget.generated.h"

class UButton;
class UStepProgressBar;

/**
 *
 */
UCLASS()
class CH3_TEAM12_API USoundSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnVolumeChanged OnMasterVolumeChanged;
	FOnVolumeChanged OnBGMVolumeChanged;
	FOnVolumeChanged OnSFXVolumeChanged;
	FOnButtonClicked OnBtnResetClicked;
	FOnButtonClicked OnBtnBackClicked;

	void UpdateWidget(float MasterVolume, float BGMVolume, float SFXVolume);

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStepProgressBar> MasterVolumeStep;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStepProgressBar> BGMVolumeStep;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStepProgressBar> SFXVolumeStep;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnReset;

	UPROPERTY(meta=(BindWidget, OptionalWidget=true))
	TObjectPtr<UButton> BtnBack;

	UFUNCTION()
	void HandleMasterVolumeChanged(float Volume) const;

	UFUNCTION()
	void HandleBGMVolumeChanged(float Volume) const;

	UFUNCTION()
	void HandleSFXVolumeChanged(float Volume) const;

	UFUNCTION()
	void HandleBtnResetClicked() const;

	UFUNCTION()
	void HandleBtnBackClicked() const;

	void SetMasterVolumeWidget(const float Volume) const;
	void SetBGMVolumeWidget(const float Volume) const;
	void SetSFXVolumeWidget(const float Volume) const;
};
