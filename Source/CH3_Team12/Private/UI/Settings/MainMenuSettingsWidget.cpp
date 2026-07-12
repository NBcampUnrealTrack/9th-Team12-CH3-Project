#include "UI/Settings/MainMenuSettingsWidget.h"

#include "Components/Button.h"
#include "UI/Settings/SoundSettingsWidget.h"

USoundSettingsWidget* UMainMenuSettingsWidget::GetSoundSettings() const
{
	return SoundSettingsWidget.Get();
}

void UMainMenuSettingsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BtnBack->OnClicked.AddDynamic(this, &UMainMenuSettingsWidget::HandleBtnBackClicked);
	BtnSoundSettings->OnClicked.AddDynamic(this, &UMainMenuSettingsWidget::HandleBtnSoundSettingsClicked);
	SoundSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
	SoundSettingsWidget->OnVisibilityChanged.AddDynamic(this, &UMainMenuSettingsWidget::HandleVisibilitySoundSettingsChanged);
}

void UMainMenuSettingsWidget::HandleBtnBackClicked()
{
	(void)OnBtnBackClicked.ExecuteIfBound();
}

void UMainMenuSettingsWidget::HandleBtnSoundSettingsClicked()
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
}
