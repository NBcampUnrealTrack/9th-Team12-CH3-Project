#include "UI/Settings/SoundSettingsPresenter.h"

#include "Framework/Subsystem/KatanaSoundManagerSubsystem.h"
#include "UI/Settings/SoundSettingsWidget.h"

void USoundSettingsPresenter::Initialize(USoundSettingsWidget* InWidget)
{
	SoundSettingsWidget = InWidget;

	if (!SoundSettingsWidget.IsValid())
		return;

	SoundSettingsWidget->OnMasterVolumeChanged.BindDynamic(this, &USoundSettingsPresenter::HandleMasterVolumeChanged);
	SoundSettingsWidget->OnBGMVolumeChanged.BindDynamic(this, &USoundSettingsPresenter::HandleBGMVolumeChanged);
	SoundSettingsWidget->OnSFXVolumeChanged.BindDynamic(this, &USoundSettingsPresenter::HandleSFXVolumeChanged);
	SoundSettingsWidget->OnUIVolumeChanged.BindDynamic(this, &USoundSettingsPresenter::HandleUIVolumeChanged);

	SoundSettingsWidget->OnBtnResetClicked.BindDynamic(this, &USoundSettingsPresenter::HandleBtnResetClicked);
	SoundSettingsWidget->OnBtnBackClicked.BindDynamic(this, &USoundSettingsPresenter::HandleBtnBackClicked);

	SoundSettingsWidget->UpdateWidget(
		UKatanaSoundManagerSubsystem::GetVolume(this, EAudioType::Master),
		UKatanaSoundManagerSubsystem::GetVolume(this, EAudioType::BGM),
		UKatanaSoundManagerSubsystem::GetVolume(this, EAudioType::SFX),
		UKatanaSoundManagerSubsystem::GetVolume(this, EAudioType::UI)
	);
}

void USoundSettingsPresenter::Dispose()
{
	if (SoundSettingsWidget.IsValid())
	{
		SoundSettingsWidget->OnMasterVolumeChanged.Unbind();
		SoundSettingsWidget->OnBGMVolumeChanged.Unbind();
		SoundSettingsWidget->OnSFXVolumeChanged.Unbind();
		SoundSettingsWidget->OnUIVolumeChanged.Unbind();
		SoundSettingsWidget->OnBtnResetClicked.Unbind();
		SoundSettingsWidget->OnBtnBackClicked.Unbind();
	}
}

void USoundSettingsPresenter::HandleMasterVolumeChanged(const float NewVolume) const
{
	UKatanaSoundManagerSubsystem::SetVolume(this, EAudioType::Master, NewVolume);
}

void USoundSettingsPresenter::HandleBGMVolumeChanged(const float NewVolume) const
{
	UKatanaSoundManagerSubsystem::SetVolume(this, EAudioType::BGM, NewVolume);
}

void USoundSettingsPresenter::HandleSFXVolumeChanged(const float NewVolume) const
{
	UKatanaSoundManagerSubsystem::SetVolume(this, EAudioType::SFX, NewVolume);
}

void USoundSettingsPresenter::HandleUIVolumeChanged(float NewVolume) const
{
	UKatanaSoundManagerSubsystem::SetVolume(this, EAudioType::UI, NewVolume);
}

void USoundSettingsPresenter::HandleBtnResetClicked() const
{
	if (!SoundSettingsWidget.IsValid())
		return;

	SoundSettingsWidget->UpdateWidget(
		UKatanaSoundManagerSubsystem::GetDefaultVolume(EAudioType::Master),
		UKatanaSoundManagerSubsystem::GetDefaultVolume(EAudioType::BGM),
		UKatanaSoundManagerSubsystem::GetDefaultVolume(EAudioType::SFX),
		UKatanaSoundManagerSubsystem::GetDefaultVolume(EAudioType::UI)
	);
}

void USoundSettingsPresenter::HandleBtnBackClicked() const
{
	SoundSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
}
