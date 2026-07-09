#include "UI/Settings/MainMenuSettingsPresenter.h"

#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"
#include "UI/Settings/MainMenuSettingsWidget.h"
#include "UI/Settings/SoundSettingsWidget.h"

void UMainMenuSettingsPresenter::Initialize(UMainMenuSettingsWidget* InWidget)
{
	MainMenuSettingsWidget = InWidget;

	if (MainMenuSettingsWidget.IsValid())
		MainMenuSettingsWidget->OnVisibilitySoundSettingsChanged.BindDynamic(
			this, &UMainMenuSettingsPresenter::HandleVisibilitySoundSettingsChanged);
}

void UMainMenuSettingsPresenter::Dispose()
{
	if (MainMenuSettingsWidget.IsValid())
		MainMenuSettingsWidget->OnVisibilitySoundSettingsChanged.Unbind();
}

void UMainMenuSettingsPresenter::HandleVisibilitySoundSettingsChanged(const ESlateVisibility InVisibility,
                                                                      UUserWidget* InWidget)
{
	USoundSettingsWidget* SoundSettingsWidget = Cast<USoundSettingsWidget>(InWidget);
	if (!SoundSettingsWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("SoundSettingsWidget is not valid"));
		return;
	}

	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(this);
	if (!UIManager)
	{
		UE_LOG(LogTemp, Error, TEXT("UIManager is not valid"));
		return;
	}

	switch (InVisibility)
	{
	case ESlateVisibility::Visible:
		{
			UIManager->RegisterSoundSettingsWidget(SoundSettingsWidget);
			break;
		}
	case ESlateVisibility::Collapsed:
		{
			UIManager->UnregisterSoundSettingsWidget(SoundSettingsWidget);
			break;
		}
	default:
		break;
	}
}
