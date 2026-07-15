#include "UI/Settings/SoundSettingsWidget.h"

#include "Components/Button.h"
#include "UI/Widget/StepProgressBar.h"

// ReSharper disable once CppMemberFunctionMayBeConst
void USoundSettingsWidget::UpdateWidget(const float MasterVolume, const float BGMVolume, const float SFXVolume)
{
	SetMasterVolumeWidget(MasterVolume);
	SetBGMVolumeWidget(BGMVolume);
	SetSFXVolumeWidget(SFXVolume);

	HandleMasterVolumeChanged(MasterVolume);
	HandleBGMVolumeChanged(BGMVolume);
	HandleSFXVolumeChanged(SFXVolume);
}

void USoundSettingsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MasterVolumeStep->OnValueChanged.AddDynamic(this, &USoundSettingsWidget::HandleMasterVolumeChanged);
	BGMVolumeStep->OnValueChanged.AddDynamic(this, &USoundSettingsWidget::HandleBGMVolumeChanged);
	SFXVolumeStep->OnValueChanged.AddDynamic(this, &USoundSettingsWidget::HandleSFXVolumeChanged);

	if (BtnReset)
		BtnReset->OnClicked.AddDynamic(this, &USoundSettingsWidget::HandleBtnResetClicked);

	if (BtnDone)
		BtnDone->OnClicked.AddDynamic(this, &USoundSettingsWidget::HandleBtnDoneClicked);
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

void USoundSettingsWidget::SetMasterVolumeWidget(const float Volume) const
{
	MasterVolumeStep->SetPercent(Volume);
}

void USoundSettingsWidget::SetBGMVolumeWidget(const float Volume) const
{
	BGMVolumeStep->SetPercent(Volume);
}

void USoundSettingsWidget::SetSFXVolumeWidget(const float Volume) const
{
	SFXVolumeStep->SetPercent(Volume);
}