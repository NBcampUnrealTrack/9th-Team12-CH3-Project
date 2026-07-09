#include "UI/Settings/SoundSettingsPresenter.h"

#include "Framework/Subsystem/KatanaSoundManagerSubsystem.h"
#include "UI/Settings/SoundSettingsWidget.h"

void USoundSettingsPresenter::Initialize(USoundSettingsWidget* InWidget)
{
	SoundManagerSubsystem = UKatanaSoundManagerSubsystem::Get(this);
	if (!SoundManagerSubsystem.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("USoundSettingsPresenter: SoundManagerSubsystem is null"));
		return;
	}

	SoundSettingsWidget = InWidget;

	if (SoundSettingsWidget.IsValid())
	{
		SoundSettingsWidget->OnMasterVolumeChanged.BindDynamic(this, &USoundSettingsPresenter::HandleMasterVolumeChanged);
		SoundSettingsWidget->OnBGMVolumeChanged.BindDynamic(this, &USoundSettingsPresenter::HandleBGMVolumeChanged);
		SoundSettingsWidget->OnSFXVolumeChanged.BindDynamic(this, &USoundSettingsPresenter::HandleSFXVolumeChanged);
	}
}

void USoundSettingsPresenter::Dispose()
{
	if (SoundSettingsWidget.IsValid())
	{
		SoundSettingsWidget->OnMasterVolumeChanged.Unbind();
		SoundSettingsWidget->OnBGMVolumeChanged.Unbind();
		SoundSettingsWidget->OnSFXVolumeChanged.Unbind();
	}
}

void USoundSettingsPresenter::HandleMasterVolumeChanged(float NewVolume) const
{
	if (!SoundManagerSubsystem.IsValid())
		return;

	UE_LOG(LogTemp, Warning, TEXT("Master Volume Changed: %f"), NewVolume);
	SoundManagerSubsystem->SetVolume(EAudioType::Master, NewVolume);
}

void USoundSettingsPresenter::HandleBGMVolumeChanged(float NewVolume) const
{
	if (!SoundManagerSubsystem.IsValid())
		return;

	UE_LOG(LogTemp, Warning, TEXT("BGM Volume Changed: %f"), NewVolume);
	SoundManagerSubsystem->SetVolume(EAudioType::BGM, NewVolume);
}

void USoundSettingsPresenter::HandleSFXVolumeChanged(float NewVolume) const
{
	if (!SoundManagerSubsystem.IsValid())
		return;

	UE_LOG(LogTemp, Warning, TEXT("SFX Volume Changed: %f"), NewVolume);
	SoundManagerSubsystem->SetVolume(EAudioType::SFX, NewVolume);
}
