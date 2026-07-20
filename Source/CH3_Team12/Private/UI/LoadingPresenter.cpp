#include "UI/LoadingPresenter.h"

#include "Framework/Subsystem/KatanaLevelSubsystem.h"
#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"
#include "UI/LoadingWidget.h"

void ULoadingPresenter::Initialize(ULoadingWidget* InWidget)
{
	LevelSubsystem = UKatanaLevelSubsystem::Get(this);

	if (!LevelSubsystem.IsValid())
		return;

	LoadingWidget = InWidget;

	if (!LoadingWidget.IsValid())
		return;

	LevelSubsystem->OnLoadingProgressUpdated.AddDynamic(this, &ULoadingPresenter::HandleModelProgressUpdated);
	LevelSubsystem->OnLoadingCompleted.AddDynamic(this, &ULoadingPresenter::HandleLoadingCompleted);
}

void ULoadingPresenter::Dispose()
{
	if (!LevelSubsystem.IsValid())
		return;

	LevelSubsystem->OnLoadingProgressUpdated.RemoveDynamic(this, &ULoadingPresenter::HandleModelProgressUpdated);
	LevelSubsystem->OnLoadingCompleted.RemoveDynamic(this, &ULoadingPresenter::HandleLoadingCompleted);
}

void ULoadingPresenter::HandleModelProgressUpdated(const float Percent) const
{
	if (LoadingWidget.IsValid())
	{
		LoadingWidget->UpdateProgress(Percent);
	}
}

void ULoadingPresenter::HandleLoadingCompleted() const
{
	UKatanaUIManagerSubsystem* UIManagerSubsystem = UKatanaUIManagerSubsystem::Get(this);

	if (!UIManagerSubsystem)
		return;

	UIManagerSubsystem->HideLoadingWidget();
}