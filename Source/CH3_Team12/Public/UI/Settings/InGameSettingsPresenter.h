#pragma once

#include "CoreMinimal.h"
#include "InGameSettingsWidget.h"
#include "Framework/PresenterInterface.h"
#include "Framework/Subsystem/KatanaSoundManagerSubsystem.h"
#include "UObject/Object.h"
#include "InGameSettingsPresenter.generated.h"

/**
 *
 */
UCLASS()
class CH3_TEAM12_API UInGameSettingsPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	virtual void Initialize(UKatanaSoundManagerSubsystem* InSubsystem, UInGameSettingsWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UKatanaSoundManagerSubsystem> SoundManagerSubsystem;

	UPROPERTY()
	TWeakObjectPtr<UInGameSettingsWidget> SettingsWidget;

	UFUNCTION()
	void OnMasterVolumeChanged(float NewVolume) const;

	UFUNCTION()
	void OnBGMVolumeChanged(float NewVolume) const;

	UFUNCTION()
	void OnSFXVolumeChanged(float NewVolume) const;

};
