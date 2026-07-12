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

	UKatanaUIManagerSubsystem* KatanaUIManagerSubsystem = UKatanaUIManagerSubsystem::Get(this);
	if (!ensure(KatanaUIManagerSubsystem))
	{
		return;
	}

	KatanaUIManagerSubsystem->RegisterInventoryWidget(InGameMenuWidget->GetInventoryWidget());
	KatanaUIManagerSubsystem->RegisterSoundSettingsWidget(InGameMenuWidget->GetSoundSettingsWidget());
}

void UInGameMenuPresenter::Dispose()
{
	if (!InGameMenuWidget.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("InGameMenuWidget Widget 이 유효하지 않습니다."));
		return;
	}

	UKatanaUIManagerSubsystem* KatanaUIManagerSubsystem = UKatanaUIManagerSubsystem::Get(this);
	if (!ensure(KatanaUIManagerSubsystem))
	{
		return;
	}

	KatanaUIManagerSubsystem->UnregisterInventoryWidget(InGameMenuWidget->GetInventoryWidget());
	KatanaUIManagerSubsystem->UnregisterSoundSettingsWidget(InGameMenuWidget->GetSoundSettingsWidget());
}
