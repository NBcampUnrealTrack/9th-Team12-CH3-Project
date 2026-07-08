#include "UI/KatanaSettingsWidget.h"

#include "UI/Widget/StepProgressBar.h"

void UKatanaSettingsWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MasterVolumeStep->OnValueChanged.AddDynamic(this, &UKatanaSettingsWidget::HandleMasterVolumeChanged);
	BGMVolumeStep->OnValueChanged.AddDynamic(this, &UKatanaSettingsWidget::HandleBGMVolumeChanged);
	SFXVolumeStep->OnValueChanged.AddDynamic(this, &UKatanaSettingsWidget::HandleSFXVolumeChanged);
}

void UKatanaSettingsWidget::HandleMasterVolumeChanged(const float Volume) const
{
	OnMasterVolumeChanged.Broadcast(Volume);
}

void UKatanaSettingsWidget::HandleBGMVolumeChanged(const float Volume) const
{
	OnBGMVolumeChanged.Broadcast(Volume);
}

void UKatanaSettingsWidget::HandleSFXVolumeChanged(const float Volume) const
{
	OnSFXVolumeChanged.Broadcast(Volume);
}
