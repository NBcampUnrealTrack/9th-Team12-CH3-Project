#include "UI/KatanaMainMenuWidget.h"

#include "Components/Button.h"

void UKatanaMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayButton->OnClicked.AddDynamic(this, &UKatanaMainMenuWidget::HandlePlayButtonClicked);
	SettingsButton->OnClicked.AddDynamic(this, &UKatanaMainMenuWidget::HandleSettingsButtonClicked);
	QuitButton->OnClicked.AddDynamic(this, &UKatanaMainMenuWidget::HandleQuitButtonClicked);
}

void UKatanaMainMenuWidget::HandlePlayButtonClicked() const
{
	OnPlayButtonClicked.Broadcast();
}

void UKatanaMainMenuWidget::HandleSettingsButtonClicked() const
{
	OnSettingsButtonClicked.Broadcast();
}

void UKatanaMainMenuWidget::HandleQuitButtonClicked() const
{
	OnQuitButtonClicked.Broadcast();
}
