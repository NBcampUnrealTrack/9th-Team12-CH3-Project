#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"

#include "Framework/KatanaSystemSettings.h"
#include "Framework/PresenterInterface.h"
#include "UI/KatanaLoadingPresenter.h"
#include "UI/KatanaLoadingWidget.h"
#include "UI/KatanaMainMenuPresenter.h"
#include "UI/KatanaMainMenuWidget.h"
#include "UI/KatanaPlayerPresenter.h"
#include "UI/KatanaPlayerWidget.h"

void UKatanaUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UKatanaSystemSettings* SystemSettings = GetDefault<UKatanaSystemSettings>();
	if (!SystemSettings)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaUIManagerSubsystem: SystemSettings is null"));
		return;
	}

	UIDataAsset = SystemSettings->UIDataAsset.LoadSynchronous();
	if (UIDataAsset.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaUIManagerSubsystem: UIDataAsset is null"));
	}
}

void UKatanaUIManagerSubsystem::ShowMainMenuWidget()
{
	UKatanaMainMenuWidget* ActiveView = OpShowUI<UKatanaMainMenuWidget>(MainMenuWidgetName);
	if (!ActiveView)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Widget 를 생성하지 못했습니다."), *MainMenuWidgetName.ToString());
		return;
	}

	UKatanaMainMenuPresenter* NewPresenter = NewObject<UKatanaMainMenuPresenter>(this);
	if (!NewPresenter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Presenter 를 생성하지 못했습니다."), *MainMenuWidgetName.ToString());
		return;
	}

	NewPresenter->Initialize(ActiveView);
	ActivePresenters.Add(MainMenuWidgetName, NewPresenter);
}

void UKatanaUIManagerSubsystem::HideMainMenuWidget()
{
	OpHideUI(MainMenuWidgetName);
}

void UKatanaUIManagerSubsystem::ShowLoadingWidget(UKatanaLevelSubsystem* InSubsystem)
{
	UKatanaLoadingWidget* ActiveView = OpShowUI<UKatanaLoadingWidget>(LoadingWidgetName);
	if (!ActiveView)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Widget 를 생성하지 못했습니다."), *LoadingWidgetName.ToString());
		return;
	}

	UKatanaLoadingPresenter* NewPresenter = NewObject<UKatanaLoadingPresenter>(this);
	if (!NewPresenter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Presenter 를 생성하지 못했습니다."), *LoadingWidgetName.ToString());
		return;
	}

	NewPresenter->Initialize(InSubsystem, ActiveView);
	ActivePresenters.Add(LoadingWidgetName, NewPresenter);
}

void UKatanaUIManagerSubsystem::HideLoadingWidget()
{
	OpHideUI(LoadingWidgetName);
}

void UKatanaUIManagerSubsystem::ShowPlayerWidget(UPlayerAttributeComponent* InAttributeComponent)
{
	UKatanaPlayerWidget* ActiveView = OpShowUI<UKatanaPlayerWidget>(PlayerWidgetName);
	if (!ActiveView) return;

	UKatanaPlayerPresenter* NewPresenter = NewObject<UKatanaPlayerPresenter>(this);
	if (!NewPresenter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Presenter 를 생성하지 못했습니다."), *PlayerWidgetName.ToString());
		return;
	}

	NewPresenter->Initialize(InAttributeComponent, ActiveView);
	ActivePresenters.Add(PlayerWidgetName, NewPresenter);
}

void UKatanaUIManagerSubsystem::HidePlayerWidget()
{
	OpHideUI(PlayerWidgetName);
}

void UKatanaUIManagerSubsystem::OpHideUI(const FName UIName)
{
	if (ActivePresenters.Contains(UIName))
	{
		if (ActivePresenters[UIName].GetInterface())
		{
			ActivePresenters[UIName]->Dispose();
		}
		ActivePresenters.Remove(UIName);
	}

	if (ActiveViews.Contains(UIName))
	{
		if (UUserWidget* TargetView = ActiveViews[UIName])
		{
			TargetView->RemoveFromParent();
		}
		ActiveViews.Remove(UIName);
	}
}