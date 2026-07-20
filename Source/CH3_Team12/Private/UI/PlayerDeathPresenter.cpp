// ReSharper disable CppMemberFunctionMayBeConst
#include "UI/PlayerDeathPresenter.h"

#include "Framework/Subsystem/KatanaLevelSubsystem.h"
#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"
#include "UI/PlayerDeathWidget.h"

void UPlayerDeathPresenter::Initialize(UPlayerDeathWidget* InWidget)
{
	PlayerDeathWidget = InWidget;

	if (!PlayerDeathWidget.IsValid())
		return;

	PlayerDeathWidget->OnRestartButtonClicked.BindDynamic(this, &UPlayerDeathPresenter::HandleRestartButtonClicked);
	PlayerDeathWidget->OnMainMenuButtonClicked.BindDynamic(this, &UPlayerDeathPresenter::HandleMainMenuButtonClicked);
}

void UPlayerDeathPresenter::Dispose()
{
	if (!PlayerDeathWidget.IsValid())
		return;

	PlayerDeathWidget->OnRestartButtonClicked.Unbind();
	PlayerDeathWidget->OnMainMenuButtonClicked.Unbind();
}

void UPlayerDeathPresenter::HandleRestartButtonClicked()
{
	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(GetWorld());
	if (!UIManager)
		return;

	UIManager->HidePlayerDeathWidget();

	UKatanaLevelSubsystem* LevelManager = UKatanaLevelSubsystem::Get(GetWorld());
	if (!LevelManager)
		return;

	LevelManager->LoadLevel("LobbyLevel"); // 타이틀 화면으로 이동
}

void UPlayerDeathPresenter::HandleMainMenuButtonClicked()
{
	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(GetWorld());
	if (!UIManager)
		return;

	UIManager->HidePlayerDeathWidget();

	UKatanaLevelSubsystem* LevelManager = UKatanaLevelSubsystem::Get(GetWorld());
	if (!LevelManager)
		return;

	LevelManager->LoadLevel("MainMenuLevel"); // 타이틀 화면으로 이동
}
