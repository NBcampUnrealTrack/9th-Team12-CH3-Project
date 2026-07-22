#include "UI/InGameMenuPresenter.h"

#include "Framework/Subsystem/KatanaLevelSubsystem.h"
#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"
#include "UI/InGameMenuWidget.h"

void UInGameMenuPresenter::Initialize(UInGameMenuWidget* InWidget)
{
	InGameMenuWidget = InWidget;

	if (!InGameMenuWidget.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("InGameMenuWidget Widget 이 유효하지 않습니다."));
		return;
	}

	UKatanaUIManagerSubsystem* UIManagerSubsystem = UKatanaUIManagerSubsystem::Get(this);
	if (!ensure(UIManagerSubsystem))
	{
		UE_LOG(LogTemp, Error, TEXT("UIManagerSubsystem 이 유효하지 않습니다."));
		return;
	}

	UIManagerSubsystem->RegisterInventoryWidget(InGameMenuWidget->GetInventoryWidget());
	UIManagerSubsystem->RegisterSoundSettingsWidget(InGameMenuWidget->GetSoundSettingsWidget());
	UIManagerSubsystem->RegisterGraphicSettingsWidget(InGameMenuWidget->GetGraphicSettingsWidget());
	UIManagerSubsystem->RegisterInputSettingsWidget(InGameMenuWidget->GetInputSettingsWidget());

	InGameMenuWidget->OnBtnMainMenuClicked.BindDynamic(this, &UInGameMenuPresenter::HandleBtnMainMenuClicked);
}

void UInGameMenuPresenter::Dispose()
{
	if (!InGameMenuWidget.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("InGameMenuWidget Widget 이 유효하지 않습니다."));
		return;
	}

	UKatanaUIManagerSubsystem* UIManagerSubsystem = UKatanaUIManagerSubsystem::Get(this);
	if (!ensure(UIManagerSubsystem))
	{
		UE_LOG(LogTemp, Error, TEXT("UIManagerSubsystem 이 유효하지 않습니다."));
		return;
	}

	UIManagerSubsystem->UnregisterInventoryWidget(InGameMenuWidget->GetInventoryWidget());
	UIManagerSubsystem->UnregisterSoundSettingsWidget(InGameMenuWidget->GetSoundSettingsWidget());
	UIManagerSubsystem->UnregisterGraphicSettingsWidget(InGameMenuWidget->GetGraphicSettingsWidget());
	UIManagerSubsystem->UnregisterInputSettingsWidget(InGameMenuWidget->GetInputSettingsWidget());

	InGameMenuWidget->OnBtnMainMenuClicked.Unbind();
}

void UInGameMenuPresenter::HandleBtnMainMenuClicked()
{
	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(GetWorld());
	if (!UIManager)
		return;

	UIManager->HideInGameMenuWidget();

	UKatanaLevelSubsystem* LevelManager = UKatanaLevelSubsystem::Get(GetWorld());
	if (!LevelManager)
		return;

	LevelManager->LoadLevel("MainMenuLevel", 0.0f); // 타이틀 화면으로 이동
}
