#include "UI/KatanaLoadingPresenter.h"

#include "Framework/Subsystem/KatanaLevelSubsystem.h"
#include "UI/KatanaLoadingWidget.h"

void UKatanaLoadingPresenter::Initialize(UKatanaLevelSubsystem* InSubsystem, UKatanaLoadingWidget* InWidget)
{
	LevelSubsystem = InSubsystem;
	LoadingWidget = InWidget;

	if (!LevelSubsystem.IsValid() || !LoadingWidget.IsValid())
		return;

	LevelSubsystem->OnLoadingProgressUpdated.AddDynamic(this, &UKatanaLoadingPresenter::OnModelProgressUpdated);
}

void UKatanaLoadingPresenter::Dispose()
{
	if (!LevelSubsystem.IsValid())
		return;

	LevelSubsystem->OnLoadingProgressUpdated.RemoveDynamic(this, &UKatanaLoadingPresenter::OnModelProgressUpdated);
}

void UKatanaLoadingPresenter::OnModelProgressUpdated(const float Percent) const
{
	if (LoadingWidget.IsValid())
	{
		LoadingWidget->UpdateProgress(Percent);
	}
}
