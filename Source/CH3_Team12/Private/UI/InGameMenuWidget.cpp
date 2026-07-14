#include "UI/InGameMenuWidget.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

UInventoryWidget* UInGameMenuWidget::GetInventoryWidget() const
{
	return InventoryWidget.Get();
}

USoundSettingsWidget* UInGameMenuWidget::GetSoundSettingsWidget() const
{
	return SoundSettingsWidget.Get();
}

UGraphicSettingsWidget* UInGameMenuWidget::GetGraphicSettingsWidget() const
{
	return GraphicSettingsWidget.Get();
}

void UInGameMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BtnInventory->OnClicked.AddDynamic(this, &UInGameMenuWidget::HandleBtnInventoryClicked);
	BtnSettings->OnClicked.AddDynamic(this, &UInGameMenuWidget::HandleBtnSettingsClicked);

	BtnChildSoundSettings->OnClicked.AddDynamic(this, &UInGameMenuWidget::HandleBtnChildSoundSettingsClicked);
	BtnChildGraphicsSettings->OnClicked.AddDynamic(this, &UInGameMenuWidget::HandleBtnChildGraphicsSettingsClicked);
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

// ReSharper disable once CppMemberFunctionMayBeConst
void UInGameMenuWidget::WidgetSwitcherChanged(const int32 ActiveWidgetIndex)
{
	WidgetSwitcher->SetActiveWidgetIndex(ActiveWidgetIndex);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UInGameMenuWidget::SettingsWidgetSwitcherChanged(const int32 ActiveWidgetIndex)
{
	SettingsWidgetSwitcher->SetActiveWidgetIndex(ActiveWidgetIndex);
}

