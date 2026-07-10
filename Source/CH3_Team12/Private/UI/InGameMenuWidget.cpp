#include "UI/InGameMenuWidget.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

void UInGameMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BtnInventory->OnClicked.AddDynamic(this, &UInGameMenuWidget::HandleBtnInventoryClicked);
	BtnSettings->OnClicked.AddDynamic(this, &UInGameMenuWidget::HandleBtnSettingsClicked);

	BtnChildGraphicsSettings->OnClicked.AddDynamic(this, &UInGameMenuWidget::HandleBtnChildGraphicsSettingsClicked);
	BtnChildSoundSettings->OnClicked.AddDynamic(this, &UInGameMenuWidget::HandleBtnChildSoundSettingsClicked);
}

void UInGameMenuWidget::HandleBtnInventoryClicked()
{
	WidgetSwitcherChanged(0);
}

void UInGameMenuWidget::HandleBtnSettingsClicked()
{
	WidgetSwitcherChanged(1);
}

void UInGameMenuWidget::HandleBtnChildSoundSettingsClicked()
{
	SettingsWidgetSwitcherChanged(0);
}

void UInGameMenuWidget::HandleBtnChildGraphicsSettingsClicked()
{
	SettingsWidgetSwitcherChanged(1);
}

void UInGameMenuWidget::WidgetSwitcherChanged(const int32 ActiveWidgetIndex)
{
	WidgetSwitcher->SetActiveWidgetIndex(ActiveWidgetIndex);

}

void UInGameMenuWidget::SettingsWidgetSwitcherChanged(const int32 ActiveWidgetIndex)
{
	SettingsWidgetSwitcher->SetActiveWidgetIndex(ActiveWidgetIndex);
}

