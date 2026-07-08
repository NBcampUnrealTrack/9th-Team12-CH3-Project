#include "UI/Settings/InGameSettingsWidget.h"

#include "UI/Widget/StepProgressBar.h"

void UInGameSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MasterVolumeStep->OnValueChanged.AddDynamic(this, &UInGameSettingsWidget::HandleMasterVolumeChanged);
	BGMVolumeStep->OnValueChanged.AddDynamic(this, &UInGameSettingsWidget::HandleBGMVolumeChanged);
	SFXVolumeStep->OnValueChanged.AddDynamic(this, &UInGameSettingsWidget::HandleSFXVolumeChanged);
}

void UInGameSettingsWidget::HandleMasterVolumeChanged(const float Volume) const
{
	OnMasterVolumeChanged.Broadcast(Volume);
}

void UInGameSettingsWidget::HandleBGMVolumeChanged(const float Volume) const
{
	OnBGMVolumeChanged.Broadcast(Volume);
}

void UInGameSettingsWidget::HandleSFXVolumeChanged(const float Volume) const
{
	OnSFXVolumeChanged.Broadcast(Volume);
}
