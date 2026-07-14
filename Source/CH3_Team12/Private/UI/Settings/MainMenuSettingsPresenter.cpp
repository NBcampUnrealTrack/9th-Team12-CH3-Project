#include "UI/Settings/MainMenuSettingsPresenter.h"

#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"
#include "UI/Settings/MainMenuSettingsWidget.h"

void UMainMenuSettingsPresenter::Initialize(UMainMenuSettingsWidget* InWidget)
{
	MainMenuSettingsWidget = InWidget;

	if (!MainMenuSettingsWidget.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("MainMenuSettingsWidget is not valid"));
		return;
	}

	MainMenuSettingsWidget->OnBtnBackClicked.BindDynamic(this, &UMainMenuSettingsPresenter::HandleBtnBackClicked);

	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(this);
	if (!UIManager)
	{
		UE_LOG(LogTemp, Error, TEXT("UIManager is not valid"));
		return;
	}

	UIManager->RegisterSoundSettingsWidget(MainMenuSettingsWidget->GetSoundSettings());
	UIManager->RegisterGraphicSettingsWidget(MainMenuSettingsWidget->GetGraphicSettings());
}

void UMainMenuSettingsPresenter::Dispose()
{
	if (!MainMenuSettingsWidget.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("MainMenuSettingsWidget is not valid"));
		return;
	}

	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(this);
	if (!UIManager)
	{
		UE_LOG(LogTemp, Error, TEXT("UIManager is not valid"));
		return;
	}

	UIManager->UnregisterSoundSettingsWidget(MainMenuSettingsWidget->GetSoundSettings());
}

void UMainMenuSettingsPresenter::HandleBtnBackClicked()
{
	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(this);
	if (!UIManager)
	{
		UE_LOG(LogTemp, Error, TEXT("UIManager is not valid"));
		return;
	}

	UIManager->HideMainMenuSettingsWidget();
}
