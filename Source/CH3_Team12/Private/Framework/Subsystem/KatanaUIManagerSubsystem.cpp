#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"

#include "Framework/KatanaSystemSettings.h"
#include "Framework/PresenterInterface.h"
#include "UI/LoadingPresenter.h"
#include "UI/LoadingWidget.h"
#include "UI/MainMenuPresenter.h"
#include "UI/MainMenuWidget.h"
#include "UI/HUD/PlayerPresenter.h"
#include "UI/HUD/PlayerWidget.h"
#include "UI/Settings/InGameSettingsPresenter.h"
#include "UI/Settings/InGameSettingsWidget.h"

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
	UMainMenuWidget* ActiveView = OpShowUI<UMainMenuWidget>(MainMenuWidgetName);
	if (!ActiveView)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Widget 를 생성하지 못했습니다."), *MainMenuWidgetName.ToString());
		return;
	}

	UMainMenuPresenter* NewPresenter = NewObject<UMainMenuPresenter>(this);
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

void UKatanaUIManagerSubsystem::ShowSettingsWidget(UKatanaSoundManagerSubsystem* InSubsystem)
{
	UInGameSettingsWidget* ActiveView = OpShowUI<UInGameSettingsWidget>(SettingsWidgetName);
	if (!ActiveView)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Widget 를 생성하지 못했습니다."), *SettingsWidgetName.ToString());
		return;
	}

	UInGameSettingsPresenter* NewPresenter = NewObject<UInGameSettingsPresenter>(this);
	if (!NewPresenter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Presenter 를 생성하지 못했습니다."), *SettingsWidgetName.ToString());
		return;
	}

	NewPresenter->Initialize(InSubsystem, ActiveView);
	ActivePresenters.Add(SettingsWidgetName, NewPresenter);
}

void UKatanaUIManagerSubsystem::HideSettingsWidget()
{
	OpHideUI(SettingsWidgetName);
}

void UKatanaUIManagerSubsystem::ShowLoadingWidget(UKatanaLevelSubsystem* InSubsystem)
{
	ULoadingWidget* ActiveView = OpShowUI<ULoadingWidget>(LoadingWidgetName);
	if (!ActiveView)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Widget 를 생성하지 못했습니다."), *LoadingWidgetName.ToString());
		return;
	}

	ULoadingPresenter* NewPresenter = NewObject<ULoadingPresenter>(this);
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
	UPlayerWidget* ActiveView = OpShowUI<UPlayerWidget>(PlayerWidgetName);
	if (!ActiveView) return;

	UPlayerPresenter* NewPresenter = NewObject<UPlayerPresenter>(this);
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