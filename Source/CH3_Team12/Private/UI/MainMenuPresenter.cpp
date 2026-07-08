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

	MainMenuWidget->OnPlayButtonClicked.AddDynamic(this, &UMainMenuPresenter::OnPlayButtonClicked);
	MainMenuWidget->OnSettingsButtonClicked.AddDynamic(this, &UMainMenuPresenter::OnSettingsButtonClicked);
	MainMenuWidget->OnQuitButtonClicked.AddDynamic(this, &UMainMenuPresenter::OnQuitButtonClicked);
}

void UMainMenuPresenter::Dispose()
{
	if (!MainMenuWidget.IsValid())
		return;

	MainMenuWidget->OnPlayButtonClicked.RemoveDynamic(this, &UMainMenuPresenter::OnPlayButtonClicked);
	MainMenuWidget->OnSettingsButtonClicked.RemoveDynamic(this, &UMainMenuPresenter::OnSettingsButtonClicked);
	MainMenuWidget->OnQuitButtonClicked.RemoveDynamic(this, &UMainMenuPresenter::OnQuitButtonClicked);
}

void UMainMenuPresenter::OnPlayButtonClicked() const
{
	UKatanaLevelSubsystem* LevelSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UKatanaLevelSubsystem>();
	LevelSubsystem->LoadLevel("LobbyLevel");
}

void UMainMenuPresenter::OnSettingsButtonClicked() const
{
	UE_LOG(LogTemp, Log, TEXT("OnSettingsButtonClicked called"));

	const APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerController is null"));
		return;
	}

	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("LocalPlayer is null"));
		return;
	}

	UKatanaSoundManagerSubsystem* SoundManager = GetWorld()->GetGameInstance()->GetSubsystem<UKatanaSoundManagerSubsystem>();
	if (!SoundManager)
	{
		UE_LOG(LogTemp, Error, TEXT("SoundManager is null"));
		return;
	}

	UKatanaUIManagerSubsystem* UIManager = LocalPlayer->GetSubsystem<UKatanaUIManagerSubsystem>();
	if (!UIManager)
	{
		UE_LOG(LogTemp, Error, TEXT("UIManager is null"));
		return;
	}

	UIManager->ShowSettingsWidget(SoundManager);
}

void UMainMenuPresenter::OnQuitButtonClicked() const
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!PlayerController) return;

	// 게임 인스턴스, 플레이어 컨트롤러, 강제 종료 여부를 던져 플랫폼 맞춤형으로 안전하게 꺼지게 만듭니다.
	UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, false);
}
