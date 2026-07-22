#include "UI/Settings/GraphicSettingsPresenter.h"

#include "Framework/Subsystem/KatanaGraphicManagerSubsystem.h"
#include "UI/Settings/GraphicSettingsWidget.h"

void UGraphicSettingsPresenter::Initialize(UGraphicSettingsWidget* InWidget)
{
	GraphicSettingsWidget = InWidget;

	if (!GraphicSettingsWidget.IsValid())
	{
		return;
	}

	GraphicSettingsWidget->OnWindowModeChanged.BindDynamic(this, &UGraphicSettingsPresenter::HandleWindowModeChanged);
	GraphicSettingsWidget->OnResolutionChanged.BindDynamic(this, &UGraphicSettingsPresenter::HandleResolutionChanged);
	GraphicSettingsWidget->OnQualityChanged.BindDynamic(this, &UGraphicSettingsPresenter::HandleQualityChanged);
	GraphicSettingsWidget->OnVSyncChanged.BindDynamic(this, &UGraphicSettingsPresenter::HandleVSyncChanged);
	GraphicSettingsWidget->OnRefreshRateChanged.BindDynamic(this, &UGraphicSettingsPresenter::HandleRefreshRateChanged);

	GraphicSettingsWidget->OnBtnResetClicked.BindDynamic(this, &UGraphicSettingsPresenter::HandleBtnResetClicked);
	GraphicSettingsWidget->OnBtnDoneClicked.BindDynamic(this, &UGraphicSettingsPresenter::HandleBtnDoneClicked);
	GraphicSettingsWidget->OnBtnBackClicked.BindDynamic(this, &UGraphicSettingsPresenter::HandleBtnBackClicked);

	GraphicManagerSubsystem = UKatanaGraphicManagerSubsystem::Get(this);
	if (!GraphicManagerSubsystem.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("UGraphicSettingsPresenter: GraphicManagerSubsystem is null"));
		return;
	}

	PendingWindowMode = GraphicManagerSubsystem->GetWindowMode();
	PendingResolution = GraphicManagerSubsystem->GetResolution();
	PendingQuality = GraphicManagerSubsystem->GetGraphicQuality();
	bPendingVSync = GraphicManagerSubsystem->IsVSyncEnabled();
	PendingRefreshRate = GraphicManagerSubsystem->GetFrameRateLimit();

	GraphicSettingsWidget->UpdateWidget(
		PendingWindowMode,
		PendingResolution,
		PendingQuality,
		bPendingVSync,
		PendingRefreshRate
	);
}

void UGraphicSettingsPresenter::Dispose()
{
	if (!GraphicSettingsWidget.IsValid())
		return;

	GraphicSettingsWidget->OnWindowModeChanged.Unbind();
	GraphicSettingsWidget->OnResolutionChanged.Unbind();
	GraphicSettingsWidget->OnQualityChanged.Unbind();
	GraphicSettingsWidget->OnVSyncChanged.Unbind();
	GraphicSettingsWidget->OnRefreshRateChanged.Unbind();
	GraphicSettingsWidget->OnBtnResetClicked.Unbind();
	GraphicSettingsWidget->OnBtnDoneClicked.Unbind();
	GraphicSettingsWidget->OnBtnBackClicked.Unbind();
}

void UGraphicSettingsPresenter::HandleWindowModeChanged(const EKatanaWindowMode NewWindowMode)
{
	PendingWindowMode = NewWindowMode;

	if (NewWindowMode != EKatanaWindowMode::Windowed)
	{
		if (const UGameUserSettings* UserSettings = UGameUserSettings::GetGameUserSettings())
		{
			PendingResolution = UserSettings->GetDesktopResolution();
		}
	}
}

void UGraphicSettingsPresenter::HandleResolutionChanged(const FIntPoint NewResolution)
{
	PendingResolution = NewResolution;
}

void UGraphicSettingsPresenter::HandleQualityChanged(const EKatanaGraphicQuality NewQuality)
{
	PendingQuality = NewQuality;
}

void UGraphicSettingsPresenter::HandleVSyncChanged(const bool bIsVSync)
{
	bPendingVSync = bIsVSync;
}

void UGraphicSettingsPresenter::HandleRefreshRateChanged(const int32 NewRefreshRate)
{
	PendingRefreshRate = NewRefreshRate;
}

void UGraphicSettingsPresenter::HandleBtnResetClicked()
{
	if (!GraphicSettingsWidget.IsValid() || !GraphicManagerSubsystem.IsValid())
		return;

	PendingWindowMode = FKatanaGraphicSettingDefaults::WindowMode;
	PendingResolution = FKatanaGraphicSettingDefaults::Resolution;
	PendingQuality = FKatanaGraphicSettingDefaults::Quality;
	bPendingVSync = FKatanaGraphicSettingDefaults::bVSync;
	PendingRefreshRate = FKatanaGraphicSettingDefaults::FrameRateLimit;

	GraphicSettingsWidget->UpdateWidget(
		PendingWindowMode,
		PendingResolution,
		PendingQuality,
		bPendingVSync,
		PendingRefreshRate
	);
}

void UGraphicSettingsPresenter::HandleBtnDoneClicked() const
{
	if (!GraphicSettingsWidget.IsValid() || !GraphicManagerSubsystem.IsValid())
		return;

	GraphicManagerSubsystem->SetWindowMode(PendingWindowMode);
	GraphicManagerSubsystem->SetResolution(PendingResolution);
	GraphicManagerSubsystem->SetGraphicQuality(PendingQuality);
	GraphicManagerSubsystem->SetVSyncEnabled(bPendingVSync);
	GraphicManagerSubsystem->SetFrameRateLimit(PendingRefreshRate);

	GraphicManagerSubsystem->ApplyAndSaveGraphicSettings();

	if (GraphicSettingsWidget->bDoneAfterCollapsed)
		GraphicSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UGraphicSettingsPresenter::HandleBtnBackClicked() const
{
	GraphicSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
}
