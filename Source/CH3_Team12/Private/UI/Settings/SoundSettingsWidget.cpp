#include "UI/Settings/SoundSettingsWidget.h"

#include "UI/Widget/StepProgressBar.h"

void USoundSettingsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MasterVolumeStep->OnValueChanged.AddDynamic(this, &USoundSettingsWidget::HandleMasterVolumeChanged);
	BGMVolumeStep->OnValueChanged.AddDynamic(this, &USoundSettingsWidget::HandleBGMVolumeChanged);
	SFXVolumeStep->OnValueChanged.AddDynamic(this, &USoundSettingsWidget::HandleSFXVolumeChanged);
}

void USoundSettingsWidget::HandleMasterVolumeChanged(const float Volume) const
{
	(void)OnMasterVolumeChanged.ExecuteIfBound(Volume);
}

void USoundSettingsWidget::HandleBGMVolumeChanged(const float Volume) const
{
	(void)OnBGMVolumeChanged.ExecuteIfBound(Volume);
}

void USoundSettingsWidget::HandleSFXVolumeChanged(const float Volume) const
{
	(void)OnSFXVolumeChanged.ExecuteIfBound(Volume);
}
