#include "UI/MainMenuWidget.h"

#include "Components/Button.h"
#include "Framework/Subsystem/KatanaSoundManagerSubsystem.h"

void UMainMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	PlayButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandlePlayButtonClicked);
	SettingsButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleSettingsButtonClicked);
	QuitButton->OnClicked.AddDynamic(this, &UMainMenuWidget::HandleQuitButtonClicked);
}

void UMainMenuWidget::HandlePlayButtonClicked() const
{
	UKatanaSoundManagerSubsystem::PlaySound2D(this, EAudioType::UI, ClickSound);
	(void)OnPlayButtonClicked.ExecuteIfBound();
}

void UMainMenuWidget::HandleSettingsButtonClicked() const
{
	UKatanaSoundManagerSubsystem::PlaySound2D(this, EAudioType::UI, ClickSound);
	(void)OnSettingsButtonClicked.ExecuteIfBound();
}

void UMainMenuWidget::HandleQuitButtonClicked() const
{
	UKatanaSoundManagerSubsystem::PlaySound2D(this, EAudioType::UI, ClickSound);
	(void)OnQuitButtonClicked.ExecuteIfBound();
}
