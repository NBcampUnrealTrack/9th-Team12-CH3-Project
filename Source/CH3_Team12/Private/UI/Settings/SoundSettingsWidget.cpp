#include "UI/Settings/SoundSettingsWidget.h"

#include "Components/Button.h"
#include "Framework/Subsystem/KatanaSoundManagerSubsystem.h"
#include "UI/Widget/StepProgressBar.h"

// ReSharper disable once CppMemberFunctionMayBeConst
void USoundSettingsWidget::UpdateWidget(const float MasterVolume, const float BGMVolume, const float SFXVolume, const float UIVolume)
{
	SetMasterVolumeWidget(MasterVolume);
	SetBGMVolumeWidget(BGMVolume);
	SetSFXVolumeWidget(SFXVolume);
	SetUIVolumeWidget(UIVolume);

	(void)OnMasterVolumeChanged.ExecuteIfBound(MasterVolume);
	(void)OnBGMVolumeChanged.ExecuteIfBound(BGMVolume);
	(void)OnSFXVolumeChanged.ExecuteIfBound(SFXVolume);
	(void)OnUIVolumeChanged.ExecuteIfBound(UIVolume);
}

void USoundSettingsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MasterVolumeStep->OnValueChanged.AddDynamic(this, &USoundSettingsWidget::HandleMasterVolumeChanged);
	BGMVolumeStep->OnValueChanged.AddDynamic(this, &USoundSettingsWidget::HandleBGMVolumeChanged);
	SFXVolumeStep->OnValueChanged.AddDynamic(this, &USoundSettingsWidget::HandleSFXVolumeChanged);
	UIVolumeStep->OnValueChanged.AddDynamic(this, &USoundSettingsWidget::HandleUIVolumeChanged);

	BtnReset->OnClicked.AddDynamic(this, &USoundSettingsWidget::HandleBtnResetClicked);

	if (BtnBack)
		BtnBack->OnClicked.AddDynamic(this, &USoundSettingsWidget::HandleBtnBackClicked);
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

void USoundSettingsWidget::HandleUIVolumeChanged(float Volume) const
{
	(void)OnUIVolumeChanged.ExecuteIfBound(Volume);
}

void USoundSettingsWidget::HandleBtnResetClicked() const
{
	UKatanaSoundManagerSubsystem::PlaySound2D(this, EAudioType::UI, ClickSound);
	(void)OnBtnResetClicked.ExecuteIfBound();
}

void USoundSettingsWidget::HandleBtnBackClicked() const
{
	UKatanaSoundManagerSubsystem::PlaySound2D(this, EAudioType::UI, ClickSound);
	(void)OnBtnBackClicked.ExecuteIfBound();
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

void USoundSettingsWidget::SetUIVolumeWidget(const float Volume) const
{
	UIVolumeStep->SetPercent(Volume);
}
