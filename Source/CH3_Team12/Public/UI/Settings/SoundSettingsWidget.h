#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SoundSettingsWidget.generated.h"

class UStepProgressBar;
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnVolumeChanged, const float, Volume);

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

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStepProgressBar> MasterVolumeStep; //TODO 임시

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStepProgressBar> BGMVolumeStep; //TODO 임시

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UStepProgressBar> SFXVolumeStep; //TODO 임시

	UFUNCTION()
	void HandleMasterVolumeChanged(float Volume) const;

	UFUNCTION()
	void HandleBGMVolumeChanged(float Volume) const;

	UFUNCTION()
	void HandleSFXVolumeChanged(float Volume) const;
};
