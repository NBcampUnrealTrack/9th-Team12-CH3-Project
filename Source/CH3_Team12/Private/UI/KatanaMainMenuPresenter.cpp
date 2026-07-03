#include "UI/KatanaMainMenuPresenter.h"

#include "Engine/GameInstance.h"
#include "Framework/Subsystem/KatanaLevelSubsystem.h"
#include "UI/KatanaMainMenuWidget.h"

void UKatanaMainMenuPresenter::Initialize(UKatanaMainMenuWidget* InWidget)
{
	MainMenuWidget = InWidget;

	if (!MainMenuWidget.IsValid())
		return;

	MainMenuWidget->OnPlayButtonClicked.AddDynamic(this, &UKatanaMainMenuPresenter::OnPlayButtonClicked);
	MainMenuWidget->OnSettingsButtonClicked.AddDynamic(this, &UKatanaMainMenuPresenter::OnSettingsButtonClicked);
	MainMenuWidget->OnQuitButtonClicked.AddDynamic(this, &UKatanaMainMenuPresenter::OnQuitButtonClicked);
}

void UKatanaMainMenuPresenter::Dispose()
{
	if (!MainMenuWidget.IsValid())
		return;

	MainMenuWidget->OnPlayButtonClicked.RemoveDynamic(this, &UKatanaMainMenuPresenter::OnPlayButtonClicked);
	MainMenuWidget->OnSettingsButtonClicked.RemoveDynamic(this, &UKatanaMainMenuPresenter::OnSettingsButtonClicked);
	MainMenuWidget->OnQuitButtonClicked.RemoveDynamic(this, &UKatanaMainMenuPresenter::OnQuitButtonClicked);
}

void UKatanaMainMenuPresenter::OnPlayButtonClicked() const
{
	UKatanaLevelSubsystem* LevelSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UKatanaLevelSubsystem>();
	LevelSubsystem->LoadLevel("LobbyLevel");
}

void UKatanaMainMenuPresenter::OnSettingsButtonClicked()
{
	//TODO Setting UI 호출
}

void UKatanaMainMenuPresenter::OnQuitButtonClicked()
{
	//TODO 종료 기능 호출
}
