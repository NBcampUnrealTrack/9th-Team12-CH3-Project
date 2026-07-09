#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "SoundSettingsPresenter.generated.h"

class USoundSettingsWidget;
class UKatanaSoundManagerSubsystem;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API USoundSettingsPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	virtual void Initialize(USoundSettingsWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UKatanaSoundManagerSubsystem> SoundManagerSubsystem;

	UPROPERTY()
	TWeakObjectPtr<USoundSettingsWidget> SoundSettingsWidget;

	UFUNCTION()
	void HandleMasterVolumeChanged(float NewVolume) const;

	UFUNCTION()
	void HandleBGMVolumeChanged(float NewVolume) const;

	UFUNCTION()
	void HandleSFXVolumeChanged(float NewVolume) const;
};
