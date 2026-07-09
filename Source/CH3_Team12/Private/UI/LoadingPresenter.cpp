#include "UI/LoadingPresenter.h"

#include "Framework/Subsystem/KatanaLevelSubsystem.h"
#include "UI/LoadingWidget.h"

void ULoadingPresenter::Initialize(ULoadingWidget* InWidget)
{
	LevelSubsystem = UKatanaLevelSubsystem::Get(this);

	if (!LevelSubsystem.IsValid())
		return;

	LoadingWidget = InWidget;

	if (!LoadingWidget.IsValid())
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
