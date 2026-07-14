#include "UI/Settings/MainMenuSettingsWidget.h"

#include "Components/Button.h"
#include "UI/Settings/GraphicSettingsWidget.h"
#include "UI/Settings/SoundSettingsWidget.h"

USoundSettingsWidget* UMainMenuSettingsWidget::GetSoundSettings() const
{
	return SoundSettingsWidget.Get();
}

UGraphicSettingsWidget* UMainMenuSettingsWidget::GetGraphicSettings() const
{
	return GraphicSettingsWidget.Get();
}

void UMainMenuSettingsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BtnBack->OnClicked.AddDynamic(this, &UMainMenuSettingsWidget::HandleBtnBackClicked);

	BtnSoundSettings->OnClicked.AddDynamic(this, &UMainMenuSettingsWidget::HandleBtnSoundSettingsClicked);
	SoundSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
	SoundSettingsWidget->OnVisibilityChanged.AddDynamic(
		this, &UMainMenuSettingsWidget::HandleVisibilitySoundSettingsChanged);

	BtnGraphicSettings->OnClicked.AddDynamic(this, &UMainMenuSettingsWidget::HandleBtnGraphicSettingsClicked);
	GraphicSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
	GraphicSettingsWidget->OnVisibilityChanged.AddDynamic(
		this, &UMainMenuSettingsWidget::HandleVisibilityGraphicSettingsChanged);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UMainMenuSettingsWidget::HandleBtnBackClicked()
{
	(void)OnBtnBackClicked.ExecuteIfBound();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UMainMenuSettingsWidget::HandleBtnSoundSettingsClicked()
{
	if (BtnSoundSettings)
		BtnSoundSettings->SetIsEnabled(false);

	if (SoundSettingsWidget)
		SoundSettingsWidget->SetVisibility(ESlateVisibility::Visible);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UMainMenuSettingsWidget::HandleBtnGraphicSettingsClicked()
{
	if (BtnGraphicSettings)
		BtnGraphicSettings->SetIsEnabled(false);

	if (GraphicSettingsWidget)
		GraphicSettingsWidget->SetVisibility(ESlateVisibility::Visible);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UMainMenuSettingsWidget::HandleVisibilitySoundSettingsChanged(const ESlateVisibility InVisibility)
{
	if (InVisibility == ESlateVisibility::Collapsed)
	{
		if (BtnSoundSettings)
			BtnSoundSettings->SetIsEnabled(true);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UMainMenuSettingsWidget::HandleVisibilityGraphicSettingsChanged(const ESlateVisibility InVisibility)
{
	if (InVisibility == ESlateVisibility::Collapsed)
	{
		if (BtnGraphicSettings)
			BtnGraphicSettings->SetIsEnabled(true);
	}
}
