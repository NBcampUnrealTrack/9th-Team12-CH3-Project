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
	(void)OnPlayButtonClicked.ExecuteIfBound();
}

void UMainMenuWidget::HandleSettingsButtonClicked() const
{
	(void)OnSettingsButtonClicked.ExecuteIfBound();
}

void UMainMenuWidget::HandleQuitButtonClicked() const
{
	(void)OnQuitButtonClicked.ExecuteIfBound();
}
