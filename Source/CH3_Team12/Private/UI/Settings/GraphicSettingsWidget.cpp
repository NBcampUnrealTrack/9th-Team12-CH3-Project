#include "UI/Settings/GraphicSettingsWidget.h"

#include "Components/Button.h"
#include "Components/ComboBoxString.h"
#include "Components/CheckBox.h"
#include "Framework/Subsystem/KatanaSoundManagerSubsystem.h"

// ReSharper disable once CppMemberFunctionMayBeConst
void UGraphicSettingsWidget::UpdateWidget(const EKatanaWindowMode WindowMode, const FIntPoint Resolution,
                                          const EKatanaGraphicQuality Quality, const bool bVSync,
                                          const int32 RefreshRate)
{
	bIsUpdatingWidget = true;

	SetWindowModeWidget(WindowMode);
	SetResolutionWidget(Resolution);
	SetQualityWidget(Quality);
	SetVSyncWidget(bVSync);
	SetRefreshRateWidget(RefreshRate);

	UpdateResolutionUIState(WindowMode); //창 모드에 따라 해상도 콤보박스 활성/비활성
	bIsUpdatingWidget = false;
}

void UGraphicSettingsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	InitializeComboBoxOptions();

	WindowModeComboBox->OnSelectionChanged.AddDynamic(this, &UGraphicSettingsWidget::HandleWindowModeSelectionChanged);
	ResolutionComboBox->OnSelectionChanged.AddDynamic(this, &UGraphicSettingsWidget::HandleResolutionSelectionChanged);
	QualityComboBox->OnSelectionChanged.AddDynamic(this, &UGraphicSettingsWidget::HandleQualitySelectionChanged);
	VSyncCheckBox->OnCheckStateChanged.AddDynamic(this, &UGraphicSettingsWidget::HandleVSyncCheckStateChanged);
	RefreshRateComboBox->OnSelectionChanged.
	                     AddDynamic(this, &UGraphicSettingsWidget::HandleRefreshRateSelectionChanged);

	BtnReset->OnClicked.AddDynamic(this, &UGraphicSettingsWidget::HandleBtnResetClicked);
	BtnDone->OnClicked.AddDynamic(this, &UGraphicSettingsWidget::HandleBtnDoneClicked);

	if (BtnBack)
		BtnBack->OnClicked.AddDynamic(this, &UGraphicSettingsWidget::HandleBtnBackClicked);
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

	RefreshRateComboBox->ClearOptions();
	RefreshRateComboBox->AddOption(TEXT("Unlimited"));
	RefreshRateComboBox->AddOption(TEXT("15"));
	RefreshRateComboBox->AddOption(TEXT("30"));
	RefreshRateComboBox->AddOption(TEXT("60"));
	RefreshRateComboBox->AddOption(TEXT("90"));
	RefreshRateComboBox->AddOption(TEXT("120"));
	RefreshRateComboBox->AddOption(TEXT("144"));
	RefreshRateComboBox->AddOption(TEXT("240"));
}

// ReSharper disable once CppPassValueParameterByConstReference
void UGraphicSettingsWidget::HandleWindowModeSelectionChanged(FString SelectedItem,
                                                              ESelectInfo::Type SelectionType) const
{
	if (SelectionType == ESelectInfo::Direct) return;

	EKatanaWindowMode SelectedMode = EKatanaWindowMode::Windowed;
	if (SelectedItem == TEXT("Fullscreen")) SelectedMode = EKatanaWindowMode::Fullscreen;
	else if (SelectedItem == TEXT("Borderless")) SelectedMode = EKatanaWindowMode::WindowedFullscreen;

	(void)OnWindowModeChanged.ExecuteIfBound(SelectedMode);

	UpdateResolutionUIState(SelectedMode);
}

// ReSharper disable once CppPassValueParameterByConstReference
void UGraphicSettingsWidget::HandleResolutionSelectionChanged(FString SelectedItem,
                                                              ESelectInfo::Type SelectionType) const
{
	if (SelectionType == ESelectInfo::Direct) return;

	FString Left, Right;
	if (SelectedItem.Split(TEXT("x"), &Left, &Right))
	{
		const FIntPoint NewResolution(FCString::Atoi(*Left), FCString::Atoi(*Right));
		(void)OnResolutionChanged.ExecuteIfBound(NewResolution);
	}
}

// ReSharper disable once CppPassValueParameterByConstReference
void UGraphicSettingsWidget::HandleQualitySelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType) const
{
	if (SelectionType == ESelectInfo::Direct) return;

	EKatanaGraphicQuality SelectedQuality = EKatanaGraphicQuality::High;
	if (SelectedItem == TEXT("Low")) SelectedQuality = EKatanaGraphicQuality::Low;
	else if (SelectedItem == TEXT("Medium")) SelectedQuality = EKatanaGraphicQuality::Medium;
	else if (SelectedItem == TEXT("Epic")) SelectedQuality = EKatanaGraphicQuality::Epic;

	(void)OnQualityChanged.ExecuteIfBound(SelectedQuality);
}

void UGraphicSettingsWidget::HandleVSyncCheckStateChanged(bool bIsChecked) const
{
	if (bIsUpdatingWidget) return;

	(void)OnVSyncChanged.ExecuteIfBound(bIsChecked);
}

void UGraphicSettingsWidget::HandleRefreshRateSelectionChanged(FString SelectedItem,
                                                               ESelectInfo::Type SelectionType) const
{
	{
		if (SelectionType == ESelectInfo::Direct) return;

		int32 RefreshRate;
		if (SelectedItem == TEXT("무제한") || SelectedItem == TEXT("Unlimited"))
		{
			RefreshRate = 0;
		}
		else
		{
			RefreshRate = FCString::Atoi(*SelectedItem);
		}

		(void)OnRefreshRateChanged.ExecuteIfBound(RefreshRate);
	}
}

void UGraphicSettingsWidget::HandleBtnResetClicked() const
{
	UKatanaSoundManagerSubsystem::PlaySound2D(this, EAudioType::UI, ClickSound);
	(void)OnBtnResetClicked.ExecuteIfBound();
}

void UGraphicSettingsWidget::HandleBtnDoneClicked() const
{
	UKatanaSoundManagerSubsystem::PlaySound2D(this, EAudioType::UI, ClickSound);
	(void)OnBtnDoneClicked.ExecuteIfBound();
}

void UGraphicSettingsWidget::HandleBtnBackClicked() const
{
	UKatanaSoundManagerSubsystem::PlaySound2D(this, EAudioType::UI, ClickSound);
	(void)OnBtnBackClicked.ExecuteIfBound();
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

void UGraphicSettingsWidget::SetRefreshRateWidget(const int32 RefreshRate) const
{
	if (RefreshRate == 0)
	{
		RefreshRateComboBox->SetSelectedOption(TEXT("Unlimited"));
	}
	else
	{
		RefreshRateComboBox->SetSelectedOption(FString::Printf(TEXT("%d"), RefreshRate));
	}
}

void UGraphicSettingsWidget::UpdateResolutionUIState(const EKatanaWindowMode WindowMode) const
{
	if (WindowMode != EKatanaWindowMode::Windowed)
	{
		ResolutionComboBox->SetIsEnabled(false);

		if (const UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
		{
			const FIntPoint DesktopRes = UserSettings->GetDesktopResolution();
			SetResolutionWidget(DesktopRes);
		}
	}
	else
	{
		ResolutionComboBox->SetIsEnabled(true);
	}
}
