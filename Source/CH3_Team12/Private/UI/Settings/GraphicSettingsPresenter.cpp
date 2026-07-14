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
	GraphicSettingsWidget->OnBtnResetClicked.BindDynamic(this, &UGraphicSettingsPresenter::HandleBtnResetClicked);
	GraphicSettingsWidget->OnBtnDoneClicked.BindDynamic(this, &UGraphicSettingsPresenter::HandleBtnDoneClicked);

	GraphicManagerSubsystem = UKatanaGraphicManagerSubsystem::Get(this);
	if (!GraphicManagerSubsystem.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("UGraphicSettingsPresenter: GraphicManagerSubsystem is null"));
		return;
	}

	GraphicSettingsWidget->SetWindowModeWidget(GraphicManagerSubsystem->GetWindowMode());
	GraphicSettingsWidget->SetResolutionWidget(GraphicManagerSubsystem->GetResolution());
	GraphicSettingsWidget->SetQualityWidget(GraphicManagerSubsystem->GetGraphicQuality());
	GraphicSettingsWidget->SetVSyncWidget(GraphicManagerSubsystem->IsVSyncEnabled());
}

void UGraphicSettingsPresenter::Dispose()
{
	if (GraphicSettingsWidget.IsValid())
	{
		GraphicSettingsWidget->OnWindowModeChanged.Unbind();
		GraphicSettingsWidget->OnResolutionChanged.Unbind();
		GraphicSettingsWidget->OnQualityChanged.Unbind();
		GraphicSettingsWidget->OnVSyncChanged.Unbind();
		GraphicSettingsWidget->OnBtnResetClicked.Unbind();
		GraphicSettingsWidget->OnBtnDoneClicked.Unbind();
	}
}

void UGraphicSettingsPresenter::HandleWindowModeChanged(const EKatanaWindowMode NewWindowMode) const
{
	if (!GraphicManagerSubsystem.IsValid())
		return;

	UE_LOG(LogTemp, Warning, TEXT("Window Mode Changed to: %d"), static_cast<int32>(NewWindowMode));
	GraphicManagerSubsystem->SetWindowMode(NewWindowMode);
}

void UGraphicSettingsPresenter::HandleResolutionChanged(const FIntPoint NewResolution) const
{
	if (!GraphicManagerSubsystem.IsValid())
		return;

	UE_LOG(LogTemp, Warning, TEXT("Resolution Changed to: %dx%d"), NewResolution.X, NewResolution.Y);
	GraphicManagerSubsystem->SetResolution(NewResolution);
}

void UGraphicSettingsPresenter::HandleQualityChanged(const EKatanaGraphicQuality NewQuality) const
{
	if (!GraphicManagerSubsystem.IsValid())
		return;

	UE_LOG(LogTemp, Warning, TEXT("Graphic Quality Changed to: %d"), static_cast<int32>(NewQuality));
	GraphicManagerSubsystem->SetGraphicQuality(NewQuality);
}

void UGraphicSettingsPresenter::HandleVSyncChanged(const bool bIsVSync) const
{
	if (!GraphicManagerSubsystem.IsValid())
		return;

	UE_LOG(LogTemp, Warning, TEXT("VSync Changed to: %s"), bIsVSync ? TEXT("True") : TEXT("False"));
	GraphicManagerSubsystem->SetVSyncEnabled(bIsVSync);
}

void UGraphicSettingsPresenter::HandleBtnResetClicked() const
{
	if (!GraphicSettingsWidget.IsValid())
		return;

	GraphicSettingsWidget->ResetGraphics();
}

void UGraphicSettingsPresenter::HandleBtnDoneClicked() const
{
	if (!GraphicSettingsWidget.IsValid())
		return;

	if (GraphicManagerSubsystem.IsValid())
	{
		GraphicManagerSubsystem->ApplyAndSaveGraphicSettings();
	}

	if (GraphicSettingsWidget->bDoneAfterCollapsed)
	{
		GraphicSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}
