#include "UI/MainMenuPresenter.h"

#include "Engine/GameInstance.h"
#include "Framework/Subsystem/KatanaLevelSubsystem.h"
#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"
#include "Framework/Subsystem/KatanaSoundManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "UI/MainMenuWidget.h"

void UMainMenuPresenter::Initialize(UMainMenuWidget* InWidget)
{
	MainMenuWidget = InWidget;

	if (!MainMenuWidget.IsValid())
		return;

	MainMenuWidget->OnPlayButtonClicked.AddDynamic(this, &UMainMenuPresenter::HandlePlayButtonClicked);
	MainMenuWidget->OnSettingsButtonClicked.AddDynamic(this, &UMainMenuPresenter::HandleSettingsButtonClicked);
	MainMenuWidget->OnQuitButtonClicked.AddDynamic(this, &UMainMenuPresenter::HandleQuitButtonClicked);
}

void UMainMenuPresenter::Dispose()
{
	if (!MainMenuWidget.IsValid())
		return;

	MainMenuWidget->OnPlayButtonClicked.RemoveDynamic(this, &UMainMenuPresenter::HandlePlayButtonClicked);
	MainMenuWidget->OnSettingsButtonClicked.RemoveDynamic(this, &UMainMenuPresenter::HandleSettingsButtonClicked);
	MainMenuWidget->OnQuitButtonClicked.RemoveDynamic(this, &UMainMenuPresenter::HandleQuitButtonClicked);
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

	// 게임 인스턴스, 플레이어 컨트롤러, 강제 종료 여부를 던져 플랫폼 맞춤형으로 안전하게 꺼지게 만듭니다.
	UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, false);
}
