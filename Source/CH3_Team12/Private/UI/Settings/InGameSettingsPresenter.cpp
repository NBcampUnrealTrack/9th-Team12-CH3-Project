#include "UI/Settings/InGameSettingsPresenter.h"

void UInGameSettingsPresenter::Initialize(UKatanaSoundManagerSubsystem* InSubsystem, UInGameSettingsWidget* InWidget)
{
	SoundManagerSubsystem = InSubsystem;
	SettingsWidget = InWidget;

	if (!SettingsWidget.IsValid())
		return;

	SettingsWidget->OnMasterVolumeChanged.AddDynamic(this, &UInGameSettingsPresenter::OnMasterVolumeChanged);
	SettingsWidget->OnBGMVolumeChanged.AddDynamic(this, &UInGameSettingsPresenter::OnBGMVolumeChanged);
	SettingsWidget->OnSFXVolumeChanged.AddDynamic(this, &UInGameSettingsPresenter::OnSFXVolumeChanged);
}

void UInGameSettingsPresenter::Dispose()
{
	if (!SettingsWidget.IsValid())
		return;

	SettingsWidget->OnMasterVolumeChanged.RemoveDynamic(this, &UInGameSettingsPresenter::OnMasterVolumeChanged);
	SettingsWidget->OnBGMVolumeChanged.RemoveDynamic(this, &UInGameSettingsPresenter::OnBGMVolumeChanged);
	SettingsWidget->OnSFXVolumeChanged.RemoveDynamic(this, &UInGameSettingsPresenter::OnSFXVolumeChanged);
}

void UInGameSettingsPresenter::OnMasterVolumeChanged(const float NewVolume) const
{
	if (!SoundManagerSubsystem.IsValid())
		return;

	UE_LOG(LogTemp, Warning, TEXT("Master Volume Changed: %f"), NewVolume);
	SoundManagerSubsystem->SetVolume(EAudioType::Master, NewVolume);
}

void UInGameSettingsPresenter::OnBGMVolumeChanged(const float NewVolume) const
{
	if (!SoundManagerSubsystem.IsValid())
		return;

	UE_LOG(LogTemp, Warning, TEXT("BGM Volume Changed: %f"), NewVolume);
	SoundManagerSubsystem->SetVolume(EAudioType::BGM, NewVolume);
}

void UInGameSettingsPresenter::OnSFXVolumeChanged(const float NewVolume) const
{
	if (!SoundManagerSubsystem.IsValid())
		return;

	UE_LOG(LogTemp, Warning, TEXT("SFX Volume Changed: %f"), NewVolume);
	SoundManagerSubsystem->SetVolume(EAudioType::SFX, NewVolume);
}
