#include "UI/KatanaSettingsPresenter.h"

void UKatanaSettingsPresenter::Initialize(UKatanaSoundManagerSubsystem* InSubsystem, UKatanaSettingsWidget* InWidget)
{
	SoundManagerSubsystem = InSubsystem;
	SettingsWidget = InWidget;

	if (!SettingsWidget.IsValid())
		return;

	SettingsWidget->OnMasterVolumeChanged.AddDynamic(this, &UKatanaSettingsPresenter::OnMasterVolumeChanged);
	SettingsWidget->OnBGMVolumeChanged.AddDynamic(this, &UKatanaSettingsPresenter::OnBGMVolumeChanged);
	SettingsWidget->OnSFXVolumeChanged.AddDynamic(this, &UKatanaSettingsPresenter::OnSFXVolumeChanged);
}

void UKatanaSettingsPresenter::Dispose()
{
	if (!SettingsWidget.IsValid())
		return;

	SettingsWidget->OnMasterVolumeChanged.RemoveDynamic(this, &UKatanaSettingsPresenter::OnMasterVolumeChanged);
	SettingsWidget->OnBGMVolumeChanged.RemoveDynamic(this, &UKatanaSettingsPresenter::OnBGMVolumeChanged);
	SettingsWidget->OnSFXVolumeChanged.RemoveDynamic(this, &UKatanaSettingsPresenter::OnSFXVolumeChanged);
}

void UKatanaSettingsPresenter::OnMasterVolumeChanged(const float NewVolume) const
{
	if (!SoundManagerSubsystem.IsValid())
		return;

	UE_LOG(LogTemp, Warning, TEXT("Master Volume Changed: %f"), NewVolume);
	SoundManagerSubsystem->SetVolume(EAudioType::Master, NewVolume);
}

void UKatanaSettingsPresenter::OnBGMVolumeChanged(const float NewVolume) const
{
	if (!SoundManagerSubsystem.IsValid())
		return;

	UE_LOG(LogTemp, Warning, TEXT("BGM Volume Changed: %f"), NewVolume);
	SoundManagerSubsystem->SetVolume(EAudioType::BGM, NewVolume);
}

void UKatanaSettingsPresenter::OnSFXVolumeChanged(const float NewVolume) const
{
	if (!SoundManagerSubsystem.IsValid())
		return;

	UE_LOG(LogTemp, Warning, TEXT("SFX Volume Changed: %f"), NewVolume);
	SoundManagerSubsystem->SetVolume(EAudioType::SFX, NewVolume);
}
