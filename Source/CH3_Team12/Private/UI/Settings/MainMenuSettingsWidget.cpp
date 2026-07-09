#include "UI/Settings/MainMenuSettingsWidget.h"

#include "Components/Button.h"
#include "UI/Settings/SoundSettingsWidget.h"

void UMainMenuSettingsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (BtnSoundSettings)
		BtnSoundSettings->OnClicked.AddDynamic(this, &UMainMenuSettingsWidget::HandleClickedBtnSoundSettings);

	if (SoundSettingsWidget)
	{
		SoundSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
		SoundSettingsWidget->OnVisibilityChanged.AddDynamic(this, &UMainMenuSettingsWidget::HandleVisibilitySoundSettingsChanged);
	}
}

void UMainMenuSettingsWidget::HandleClickedBtnSoundSettings()
{
	if (BtnSoundSettings)
		BtnSoundSettings->SetIsEnabled(false);

	if (SoundSettingsWidget)
		SoundSettingsWidget->SetVisibility(ESlateVisibility::Visible);
}

void UMainMenuSettingsWidget::HandleVisibilitySoundSettingsChanged(const ESlateVisibility InVisibility)
{
	if (InVisibility == ESlateVisibility::Collapsed)
	{
		if (BtnSoundSettings)
			BtnSoundSettings->SetIsEnabled(true);
	}

	OnVisibilitySoundSettingsChanged.ExecuteIfBound(InVisibility, SoundSettingsWidget);
}
