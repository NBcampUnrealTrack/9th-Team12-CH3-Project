#include "UI/LoadingPresenter.h"

#include "Framework/Subsystem/KatanaLevelSubsystem.h"
#include "UI/LoadingWidget.h"

void ULoadingPresenter::Initialize(UKatanaLevelSubsystem* InSubsystem, ULoadingWidget* InWidget)
{
	LevelSubsystem = InSubsystem;
	LoadingWidget = InWidget;

	if (!LevelSubsystem.IsValid() || !LoadingWidget.IsValid())
		return;

	LevelSubsystem->OnLoadingProgressUpdated.AddDynamic(this, &ULoadingPresenter::OnModelProgressUpdated);
}

void ULoadingPresenter::Dispose()
{
	if (!LevelSubsystem.IsValid())
		return;

	LevelSubsystem->OnLoadingProgressUpdated.RemoveDynamic(this, &ULoadingPresenter::OnModelProgressUpdated);
}

void ULoadingPresenter::OnModelProgressUpdated(const float Percent) const
{
	if (LoadingWidget.IsValid())
	{
		LoadingWidget->UpdateProgress(Percent);
	}
}
