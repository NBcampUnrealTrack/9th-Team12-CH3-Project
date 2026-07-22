#include "UI/Settings/MainMenuSettingsPresenter.h"

#include "Framework/Subsystem/KatanaStageRecordManagerSubsystem.h"
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

	MainMenuSettingsWidget->OnBtnResetStageRecordClicked.BindDynamic(
		this, &UMainMenuSettingsPresenter::HandleBtnResetStageRecordClicked);
	MainMenuSettingsWidget->OnBtnBackClicked.BindDynamic(this, &UMainMenuSettingsPresenter::HandleBtnBackClicked);

	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(this);
	if (!UIManager)
	{
		UE_LOG(LogTemp, Error, TEXT("UIManager is not valid"));
		return;
	}

	UIManager->RegisterSoundSettingsWidget(MainMenuSettingsWidget->GetSoundSettings());
	UIManager->RegisterGraphicSettingsWidget(MainMenuSettingsWidget->GetGraphicSettings());
	UIManager->RegisterInputSettingsWidget(MainMenuSettingsWidget->GetInputSettings());
}

void UMainMenuSettingsPresenter::Dispose()
{
	if (!MainMenuSettingsWidget.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("MainMenuSettingsWidget is not valid"));
		return;
	}

	MainMenuSettingsWidget->OnBtnResetStageRecordClicked.Unbind();
	MainMenuSettingsWidget->OnBtnBackClicked.Unbind();

	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(this);
	if (!UIManager)
	{
		UE_LOG(LogTemp, Error, TEXT("UIManager is not valid"));
		return;
	}

	UIManager->UnregisterSoundSettingsWidget(MainMenuSettingsWidget->GetSoundSettings());
	UIManager->UnregisterGraphicSettingsWidget(MainMenuSettingsWidget->GetGraphicSettings());
	UIManager->UnregisterInputSettingsWidget(MainMenuSettingsWidget->GetInputSettings());
}

void UMainMenuSettingsPresenter::HandleBtnResetStageRecordClicked()
{
	UKatanaStageRecordManagerSubsystem* StageRecordManager = UKatanaStageRecordManagerSubsystem::Get(this);
	if (!StageRecordManager)
	{
		UE_LOG(LogTemp, Error, TEXT("StageRecordManager is not valid"));
		return;
	}

	StageRecordManager->ResetStageRecord();
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
