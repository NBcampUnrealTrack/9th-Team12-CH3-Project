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
	SoundSettingsWidget->OnBtnResetClicked.BindDynamic(this, &USoundSettingsPresenter::HandleBtnResetClicked);
	SoundSettingsWidget->OnBtnDoneClicked.BindDynamic(this, &USoundSettingsPresenter::HandleBtnDoneClicked);

	SoundManagerSubsystem = UKatanaSoundManagerSubsystem::Get(this);
	if (!SoundManagerSubsystem.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("USoundSettingsPresenter: SoundManagerSubsystem is null"));
		return;
	}

	SoundSettingsWidget->UpdateWidget(
		SoundManagerSubsystem->GetVolume(EAudioType::Master),
		SoundManagerSubsystem->GetVolume(EAudioType::BGM),
		SoundManagerSubsystem->GetVolume(EAudioType::SFX)
	);
}

void USoundSettingsPresenter::Dispose()
{
	if (SoundSettingsWidget.IsValid())
	{
		SoundSettingsWidget->OnMasterVolumeChanged.Unbind();
		SoundSettingsWidget->OnBGMVolumeChanged.Unbind();
		SoundSettingsWidget->OnSFXVolumeChanged.Unbind();
		SoundSettingsWidget->OnBtnResetClicked.Unbind();
		SoundSettingsWidget->OnBtnDoneClicked.Unbind();
	}
}

void USoundSettingsPresenter::HandleMasterVolumeChanged(const float NewVolume) const
{
	if (!SoundManagerSubsystem.IsValid())
		return;

	UE_LOG(LogTemp, Warning, TEXT("Master Volume Changed: %f"), NewVolume);
	SoundManagerSubsystem->SetVolume(EAudioType::Master, NewVolume);
}

void USoundSettingsPresenter::HandleBGMVolumeChanged(const float NewVolume) const
{
	if (!SoundManagerSubsystem.IsValid())
		return;

	UE_LOG(LogTemp, Warning, TEXT("BGM Volume Changed: %f"), NewVolume);
	SoundManagerSubsystem->SetVolume(EAudioType::BGM, NewVolume);
}

void USoundSettingsPresenter::HandleSFXVolumeChanged(const float NewVolume) const
{
	if (!SoundManagerSubsystem.IsValid())
		return;

	UE_LOG(LogTemp, Warning, TEXT("SFX Volume Changed: %f"), NewVolume);
	SoundManagerSubsystem->SetVolume(EAudioType::SFX, NewVolume);
}

void USoundSettingsPresenter::HandleBtnResetClicked() const
{
	if (!SoundSettingsWidget.IsValid())
		return;

	SoundSettingsWidget->UpdateWidget(
		SoundManagerSubsystem->GetDefaultVolume(EAudioType::Master),
		SoundManagerSubsystem->GetDefaultVolume(EAudioType::BGM),
		SoundManagerSubsystem->GetDefaultVolume(EAudioType::SFX)
	);
}

void USoundSettingsPresenter::HandleBtnDoneClicked() const
{
	SoundSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
}
