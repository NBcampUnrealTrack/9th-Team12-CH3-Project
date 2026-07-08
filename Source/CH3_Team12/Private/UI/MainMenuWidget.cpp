#include "UI/MainMenuWidget.h"

#include "Components/Button.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandlePlayButtonClicked);
	SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleSettingsButtonClicked);
	QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleQuitButtonClicked);
}

void UMainMenuWidget::HandlePlayButtonClicked() const
{
	OnPlayButtonClicked.Broadcast();
}

void UMainMenuWidget::HandleSettingsButtonClicked() const
{
	OnSettingsButtonClicked.Broadcast();
}

void UMainMenuWidget::HandleQuitButtonClicked() const
{
	OnQuitButtonClicked.Broadcast();
}
