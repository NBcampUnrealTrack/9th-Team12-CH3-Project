#include "UI/Settings/SoundSettingsWidget.h"

#include "UI/Widget/StepProgressBar.h"

void USoundSettingsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MasterVolumeStep->OnValueChanged.AddDynamic(this, &USoundSettingsWidget::HandleMasterVolumeChanged);
	BGMVolumeStep->OnValueChanged.AddDynamic(this, &USoundSettingsWidget::HandleBGMVolumeChanged);
	SFXVolumeStep->OnValueChanged.AddDynamic(this, &USoundSettingsWidget::HandleSFXVolumeChanged);
}

void USoundSettingsWidget::ResetVolume() const
{
	//TODO 가능 확인 필요
	MasterVolumeStep->SetPercent(1.0f);
	BGMVolumeStep->SetPercent(1.0f);
	SFXVolumeStep->SetPercent(1.0f);

	HandleMasterVolumeChanged(1.0f);
	HandleBGMVolumeChanged(1.0f);
	HandleSFXVolumeChanged(1.0f);
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

void USoundSettingsWidget::HandleBtnResetClicked() const
{
	(void)OnBtnResetClicked.ExecuteIfBound();
}

void USoundSettingsWidget::HandleBtnDoneClicked() const
{
	(void)OnBtnDoneClicked.ExecuteIfBound();
}
