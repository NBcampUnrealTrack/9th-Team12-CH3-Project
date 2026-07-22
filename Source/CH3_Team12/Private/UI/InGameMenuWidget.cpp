// ReSharper disable CppMemberFunctionMayBeConst
#include "UI/InGameMenuWidget.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Framework/Subsystem/KatanaSoundManagerSubsystem.h"

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

UInputSettingsWidget* UInGameMenuWidget::GetInputSettingsWidget() const
{
	return InputSettingsWidget.Get();
}

void UInGameMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BtnInventory->OnClicked.AddDynamic(this, &UInGameMenuWidget::HandleBtnInventoryClicked);
	BtnSettings->OnClicked.AddDynamic(this, &UInGameMenuWidget::HandleBtnSettingsClicked);
	BtnMainMenu->OnClicked.AddDynamic(this, &UInGameMenuWidget::HandleBtnMainMenuClicked);

	BtnChildSoundSettings->OnClicked.AddDynamic(this, &UInGameMenuWidget::HandleBtnChildSoundSettingsClicked);
	BtnChildGraphicsSettings->OnClicked.AddDynamic(this, &UInGameMenuWidget::HandleBtnChildGraphicsSettingsClicked);
	BtnChildInputSettings->OnClicked.AddDynamic(this, &UInGameMenuWidget::HandleBtnChildInputSettingsClicked);
}

void UInGameMenuWidget::HandleBtnInventoryClicked()
{
	UKatanaSoundManagerSubsystem::PlaySound2D(this, EAudioType::UI, ClickSound);
	WidgetSwitcherChanged(0);
}

void UInGameMenuWidget::HandleBtnSettingsClicked()
{
	UKatanaSoundManagerSubsystem::PlaySound2D(this, EAudioType::UI, ClickSound);
	WidgetSwitcherChanged(1);
}

void UInGameMenuWidget::HandleBtnMainMenuClicked()
{
	UKatanaSoundManagerSubsystem::PlaySound2D(this, EAudioType::UI, ClickSound);
	(void)OnBtnMainMenuClicked.ExecuteIfBound();
}

void UInGameMenuWidget::HandleBtnChildSoundSettingsClicked()
{
	UKatanaSoundManagerSubsystem::PlaySound2D(this, EAudioType::UI, ClickSound);
	SettingsWidgetSwitcherChanged(0);
}

void UInGameMenuWidget::HandleBtnChildGraphicsSettingsClicked()
{
	UKatanaSoundManagerSubsystem::PlaySound2D(this, EAudioType::UI, ClickSound);
	SettingsWidgetSwitcherChanged(1);
}

void UInGameMenuWidget::HandleBtnChildInputSettingsClicked()
{
	UKatanaSoundManagerSubsystem::PlaySound2D(this, EAudioType::UI, ClickSound);
	SettingsWidgetSwitcherChanged(2);
}

void UInGameMenuWidget::WidgetSwitcherChanged(const int32 ActiveWidgetIndex)
{
	WidgetSwitcher->SetActiveWidgetIndex(ActiveWidgetIndex);
}

void UInGameMenuWidget::SettingsWidgetSwitcherChanged(const int32 ActiveWidgetIndex)
{
	SettingsWidgetSwitcher->SetActiveWidgetIndex(ActiveWidgetIndex);
}

