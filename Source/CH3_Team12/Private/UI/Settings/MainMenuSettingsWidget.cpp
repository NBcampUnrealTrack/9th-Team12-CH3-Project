#include "UI/Settings/MainMenuSettingsWidget.h"

#include "Components/Button.h"
#include "UI/Settings/GraphicSettingsWidget.h"
#include "UI/Settings/InputSettingsWidget.h"
#include "UI/Settings/SoundSettingsWidget.h"

USoundSettingsWidget* UMainMenuSettingsWidget::GetSoundSettings() const
{
	return SoundSettingsWidget.Get();
}

UGraphicSettingsWidget* UMainMenuSettingsWidget::GetGraphicSettings() const
{
	return GraphicSettingsWidget.Get();
}

UInputSettingsWidget* UMainMenuSettingsWidget::GetInputSettings() const
{
	return InputSettingsWidget.Get();
}

void UMainMenuSettingsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BtnResetStageRecord->OnClicked.AddDynamic(this, &UMainMenuSettingsWidget::HandleBtnResetStageRecordClicked);
	BtnBack->OnClicked.AddDynamic(this, &UMainMenuSettingsWidget::HandleBtnBackClicked);

	BtnSoundSettings->OnClicked.AddDynamic(this, &UMainMenuSettingsWidget::HandleBtnSoundSettingsClicked);
	SoundSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
	SoundSettingsWidget->OnVisibilityChanged.AddDynamic(
		this, &UMainMenuSettingsWidget::HandleVisibilitySoundSettingsChanged);

	BtnGraphicSettings->OnClicked.AddDynamic(this, &UMainMenuSettingsWidget::HandleBtnGraphicSettingsClicked);
	GraphicSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
	GraphicSettingsWidget->OnVisibilityChanged.AddDynamic(
		this, &UMainMenuSettingsWidget::HandleVisibilityGraphicSettingsChanged);

	BtnInputSettings->OnClicked.AddDynamic(this, &UMainMenuSettingsWidget::HandleBtnInputSettingsClicked);
	InputSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
	InputSettingsWidget->OnVisibilityChanged.AddDynamic(
		this, &UMainMenuSettingsWidget::HandleVisibilityInputSettingsChanged);
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
void UMainMenuSettingsWidget::HandleBtnInputSettingsClicked()
{
	if (BtnInputSettings)
		BtnInputSettings->SetIsEnabled(false);

	if (InputSettingsWidget)
		InputSettingsWidget->SetVisibility(ESlateVisibility::Visible);
}

void UMainMenuSettingsWidget::HandleBtnResetStageRecordClicked()
{
	(void)OnBtnResetStageRecordClicked.ExecuteIfBound();
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

// ReSharper disable once CppMemberFunctionMayBeConst
void UMainMenuSettingsWidget::HandleVisibilityInputSettingsChanged(ESlateVisibility InVisibility)
{
	if (InVisibility == ESlateVisibility::Collapsed)
	{
		if (BtnInputSettings)
			BtnInputSettings->SetIsEnabled(true);
	}
}