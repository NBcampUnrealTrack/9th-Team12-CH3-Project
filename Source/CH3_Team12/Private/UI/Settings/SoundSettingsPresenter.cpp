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

	SoundManagerSubsystem = UKatanaSoundManagerSubsystem::Get(this);
	if (!SoundManagerSubsystem.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("USoundSettingsPresenter: SoundManagerSubsystem is null"));
		return;
	}

	SoundSettingsWidget->UpdateWidget(
		SoundManagerSubsystem->GetVolume(EAudioType::Master),
		SoundManagerSubsystem->GetVolume(EAudioType::BGM),
		SoundManagerSubsystem->GetVolume(EAudioType::SFX),
		SoundManagerSubsystem->GetVolume(EAudioType::UI)
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
	if (!SoundManagerSubsystem.IsValid())
		return;

	SoundManagerSubsystem->SetVolume(EAudioType::Master, NewVolume);
}

void USoundSettingsPresenter::HandleBGMVolumeChanged(const float NewVolume) const
{
	if (!SoundManagerSubsystem.IsValid())
		return;

	SoundManagerSubsystem->SetVolume(EAudioType::BGM, NewVolume);
}

void USoundSettingsPresenter::HandleSFXVolumeChanged(const float NewVolume) const
{
	if (!SoundManagerSubsystem.IsValid())
		return;

	SoundManagerSubsystem->SetVolume(EAudioType::SFX, NewVolume);
}

void USoundSettingsPresenter::HandleUIVolumeChanged(float NewVolume) const
{
	if (!SoundManagerSubsystem.IsValid())
		return;

	SoundManagerSubsystem->SetVolume(EAudioType::UI, NewVolume);
}

void USoundSettingsPresenter::HandleBtnResetClicked() const
{
	if (!SoundSettingsWidget.IsValid())
		return;

	SoundSettingsWidget->UpdateWidget(
		SoundManagerSubsystem->GetDefaultVolume(EAudioType::Master),
		SoundManagerSubsystem->GetDefaultVolume(EAudioType::BGM),
		SoundManagerSubsystem->GetDefaultVolume(EAudioType::SFX),
		SoundManagerSubsystem->GetDefaultVolume(EAudioType::UI)
	);
}

void USoundSettingsPresenter::HandleBtnBackClicked() const
{
	SoundSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
}
