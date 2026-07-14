#include "UI/Settings/GraphicSettingsWidget.h"

#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"

void UGraphicSettingsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InitializeComboBoxOptions();

	WindowModeComboBox->OnSelectionChanged.AddDynamic(this, &UGraphicSettingsWidget::HandleWindowModeSelectionChanged);
	ResolutionComboBox->OnSelectionChanged.AddDynamic(this, &UGraphicSettingsWidget::HandleResolutionSelectionChanged);
	QualityComboBox->OnSelectionChanged.AddDynamic(this, &UGraphicSettingsWidget::HandleQualitySelectionChanged);
	VSyncCheckBox->OnCheckStateChanged.AddDynamic(this, &UGraphicSettingsWidget::HandleVSyncCheckStateChanged);

	BtnReset->OnClicked.AddDynamic(this, &UGraphicSettingsWidget::HandleBtnResetClicked);
	BtnDone->OnClicked.AddDynamic(this, &UGraphicSettingsWidget::HandleBtnDoneClicked);
}

void UGraphicSettingsWidget::SetWindowModeWidget(const EKatanaWindowMode WindowMode) const
{
	FString TargetString;
	switch (WindowMode)
	{
	case EKatanaWindowMode::Fullscreen: TargetString = TEXT("Fullscreen");
		break;
	case EKatanaWindowMode::WindowedFullscreen: TargetString = TEXT("Borderless");
		break;
	case EKatanaWindowMode::Windowed: TargetString = TEXT("Windowed");
		break;
	}
	WindowModeComboBox->SetSelectedOption(TargetString);
}

void UGraphicSettingsWidget::SetResolutionWidget(const FIntPoint Resolution) const
{
	const FString TargetString = FString::Printf(TEXT("%dx%d"), Resolution.X, Resolution.Y);
	ResolutionComboBox->SetSelectedOption(TargetString);
}

void UGraphicSettingsWidget::SetQualityWidget(const EKatanaGraphicQuality Quality) const
{
	FString TargetString;
	switch (Quality)
	{
	case EKatanaGraphicQuality::Low: TargetString = TEXT("Low");
		break;
	case EKatanaGraphicQuality::Medium: TargetString = TEXT("Medium");
		break;
	case EKatanaGraphicQuality::High: TargetString = TEXT("High");
		break;
	case EKatanaGraphicQuality::Epic: TargetString = TEXT("Epic");
		break;
	}
	QualityComboBox->SetSelectedOption(TargetString);
}

void UGraphicSettingsWidget::SetVSyncWidget(const bool bIsVSync) const
{
	VSyncCheckBox->SetIsChecked(bIsVSync);
}

void UGraphicSettingsWidget::ResetGraphics() const
{
	constexpr EKatanaWindowMode DefaultWindowMode = EKatanaWindowMode::WindowedFullscreen;
	const FIntPoint DefaultResolution(1920, 1080);
	constexpr EKatanaGraphicQuality DefaultQuality = EKatanaGraphicQuality::High;
	constexpr bool bDefaultVSync = false;

	SetWindowModeWidget(DefaultWindowMode);
	SetResolutionWidget(DefaultResolution);
	SetQualityWidget(DefaultQuality);
	SetVSyncWidget(bDefaultVSync);

	(void)OnWindowModeChanged.ExecuteIfBound(DefaultWindowMode);
	(void)OnResolutionChanged.ExecuteIfBound(DefaultResolution);
	(void)OnQualityChanged.ExecuteIfBound(DefaultQuality);
	(void)OnVSyncChanged.ExecuteIfBound(bDefaultVSync);
}

void UGraphicSettingsWidget::InitializeComboBoxOptions() const
{
	WindowModeComboBox->ClearOptions();
	WindowModeComboBox->AddOption(TEXT("Fullscreen"));
	WindowModeComboBox->AddOption(TEXT("Borderless"));
	WindowModeComboBox->AddOption(TEXT("Windowed"));

	ResolutionComboBox->ClearOptions();
	ResolutionComboBox->AddOption(TEXT("3840x2160"));
	ResolutionComboBox->AddOption(TEXT("2560x1440"));
	ResolutionComboBox->AddOption(TEXT("1920x1080"));
	ResolutionComboBox->AddOption(TEXT("1600x900"));
	ResolutionComboBox->AddOption(TEXT("1280x720"));

	QualityComboBox->ClearOptions();
	QualityComboBox->AddOption(TEXT("Low"));
	QualityComboBox->AddOption(TEXT("Medium"));
	QualityComboBox->AddOption(TEXT("High"));
	QualityComboBox->AddOption(TEXT("Epic"));
}

// ReSharper disable once CppPassValueParameterByConstReference
void UGraphicSettingsWidget::HandleWindowModeSelectionChanged(FString SelectedItem,
                                                              ESelectInfo::Type SelectionType) const
{
	if (SelectionType != ESelectInfo::Direct) return;

	EKatanaWindowMode SelectedMode = EKatanaWindowMode::Windowed;
	if (SelectedItem == TEXT("Fullscreen")) SelectedMode = EKatanaWindowMode::Fullscreen;
	else if (SelectedItem == TEXT("Borderless")) SelectedMode = EKatanaWindowMode::WindowedFullscreen;

	(void)OnWindowModeChanged.ExecuteIfBound(SelectedMode);
}

// ReSharper disable once CppPassValueParameterByConstReference
void UGraphicSettingsWidget::HandleResolutionSelectionChanged(FString SelectedItem,
                                                              ESelectInfo::Type SelectionType) const
{
	if (SelectionType != ESelectInfo::Direct) return;

	FString Left, Right;
	if (SelectedItem.Split(TEXT("x"), &Left, &Right))
	{
		FIntPoint NewResolution(FCString::Atoi(*Left), FCString::Atoi(*Right));
		(void)OnResolutionChanged.ExecuteIfBound(NewResolution);
	}
}

// ReSharper disable once CppPassValueParameterByConstReference
void UGraphicSettingsWidget::HandleQualitySelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType) const
{
	if (SelectionType != ESelectInfo::Direct) return;

	EKatanaGraphicQuality SelectedQuality = EKatanaGraphicQuality::High;
	if (SelectedItem == TEXT("Low")) SelectedQuality = EKatanaGraphicQuality::Low;
	else if (SelectedItem == TEXT("Medium")) SelectedQuality = EKatanaGraphicQuality::Medium;
	else if (SelectedItem == TEXT("Epic")) SelectedQuality = EKatanaGraphicQuality::Epic;

	(void)OnQualityChanged.ExecuteIfBound(SelectedQuality);
}

void UGraphicSettingsWidget::HandleVSyncCheckStateChanged(bool bIsChecked) const
{
	(void)OnVSyncChanged.ExecuteIfBound(bIsChecked);
}

void UGraphicSettingsWidget::HandleBtnResetClicked() const
{
	(void)OnBtnResetClicked.ExecuteIfBound();
}

void UGraphicSettingsWidget::HandleBtnDoneClicked() const
{
	(void)OnBtnDoneClicked.ExecuteIfBound();
}
