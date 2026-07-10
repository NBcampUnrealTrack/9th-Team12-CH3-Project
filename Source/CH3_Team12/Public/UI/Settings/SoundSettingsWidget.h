#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SoundSettingsWidget.generated.h"

class UButton;
class UStepProgressBar;
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnVolumeChanged, const float, Volume);
DECLARE_DYNAMIC_DELEGATE(FOnBtnClicked);

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
	FOnBtnClicked OnBtnResetClicked;
	FOnBtnClicked OnBtnDoneClicked;

	void ResetVolume() const;

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStepProgressBar> MasterVolumeStep; //TODO 임시

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStepProgressBar> BGMVolumeStep; //TODO 임시

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStepProgressBar> SFXVolumeStep; //TODO 임시

	UPROPERTY(meta=(BindWidget, OptionalWidget=true))
	TObjectPtr<UButton> BtnReset;

	UPROPERTY(meta=(BindWidget, OptionalWidget=true))
	TObjectPtr<UButton> BtnDone;

	UFUNCTION()
	void HandleMasterVolumeChanged(float Volume) const;

	UFUNCTION()
	void HandleBGMVolumeChanged(float Volume) const;

	UFUNCTION()
	void HandleSFXVolumeChanged(float Volume) const;

	UFUNCTION()
	void HandleBtnResetClicked() const;

	UFUNCTION()
	void HandleBtnDoneClicked() const;
};
