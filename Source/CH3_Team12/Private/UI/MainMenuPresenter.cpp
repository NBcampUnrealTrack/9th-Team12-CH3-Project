#include "UI/MainMenuPresenter.h"

#include "Engine/GameInstance.h"
#include "Framework/Subsystem/KatanaLevelSubsystem.h"
#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MainMenuWidget.h"

void UMainMenuPresenter::Initialize(UMainMenuWidget* InWidget)
{
	MainMenuWidget = InWidget;

	if (!MainMenuWidget.IsValid())
		return;

	MainMenuWidget->OnPlayButtonClicked.BindDynamic(this, &UMainMenuPresenter::HandlePlayButtonClicked);
	MainMenuWidget->OnSettingsButtonClicked.BindDynamic(this, &UMainMenuPresenter::HandleSettingsButtonClicked);
	MainMenuWidget->OnQuitButtonClicked.BindDynamic(this, &UMainMenuPresenter::HandleQuitButtonClicked);
}

void UMainMenuPresenter::Dispose()
{
	if (MainMenuWidget.IsValid())
	{
		MainMenuWidget->OnPlayButtonClicked.Unbind();
		MainMenuWidget->OnSettingsButtonClicked.Unbind();
		MainMenuWidget->OnQuitButtonClicked.Unbind();
	}
}

void UMainMenuPresenter::HandlePlayButtonClicked() const
{
	UKatanaLevelSubsystem* LevelSubsystem = UKatanaLevelSubsystem::Get(this);
	if (!LevelSubsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("LevelSubsystem is null"));
		return;
	}

	LevelSubsystem->LoadLevel("LobbyLevel");

	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(this);
	if (!UIManager)
	{
		UE_LOG(LogTemp, Error, TEXT("UIManager is null"));
		return;
	}

	UIManager->HideMainMenuWidget();
}

void UMainMenuPresenter::HandleSettingsButtonClicked() const
{
	UE_LOG(LogTemp, Log, TEXT("OnSettingsButtonClicked called"));

	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(this);
	if (!UIManager)
	{
		UE_LOG(LogTemp, Error, TEXT("UIManager is null"));
		return;
	}

	UIManager->ShowMainMenuSettingsWidget();
}

void UMainMenuPresenter::HandleQuitButtonClicked() const
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController) return;

	UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, false);
}
