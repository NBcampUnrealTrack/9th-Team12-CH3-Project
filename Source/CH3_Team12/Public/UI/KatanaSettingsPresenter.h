#pragma once

#include "CoreMinimal.h"
#include "KatanaSettingsWidget.h"
#include "Framework/PresenterInterface.h"
#include "Framework/Subsystem/KatanaSoundManagerSubsystem.h"
#include "UObject/Object.h"
#include "KatanaSettingsPresenter.generated.h"

/**
 *
 */
UCLASS()
class CH3_TEAM12_API UKatanaSettingsPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	virtual void Initialize(UKatanaSoundManagerSubsystem* InSubsystem, UKatanaSettingsWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UKatanaSoundManagerSubsystem> SoundManagerSubsystem;

	UPROPERTY()
	TWeakObjectPtr<UKatanaSettingsWidget> SettingsWidget;

	UFUNCTION()
	void OnMasterVolumeChanged(float NewVolume) const;

	UFUNCTION()
	void OnBGMVolumeChanged(float NewVolume) const;

	UFUNCTION()
	void OnSFXVolumeChanged(float NewVolume) const;

};
