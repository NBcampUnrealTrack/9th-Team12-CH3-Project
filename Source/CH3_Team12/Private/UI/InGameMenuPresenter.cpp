#include "UI/InGameMenuPresenter.h"

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
}
