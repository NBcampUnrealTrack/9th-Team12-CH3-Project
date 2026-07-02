#include "Framework/KatanaUIManagerSubsystem.h"

#include "Framework/KatanaSystemSettings.h"
#include "Framework/PresenterInterface.h"

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

// void UUIManagerSubsystem::ShowPlayerHUD(const TScriptInterface<IExampleModelInterface>& InModel)
// {
// 	UE_LOG(LogTemp, Warning, TEXT("ShowPlayerHUD가 정상적으로 호출되었습니다."));
//
// 	const FName UIName = FName("ExampleHUD");
// 	UExampleUserWidget* ActiveView = OpShowUI<UExampleUserWidget>(UIName);
// 	if (!ActiveView) return;
//
// 	UExampleHUDPresenter* NewPresenter = NewObject<UExampleHUDPresenter>(this);
// 	if (!NewPresenter)
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("%s Presenter 를 생성하지 못했습니다."), *UIName.ToString());
// 		return;
// 	}
//
// 	NewPresenter->Initialize(InModel, ActiveView);
// 	ActivePresenters.Add(UIName, NewPresenter);
// }
//
// void UUIManagerSubsystem::HidePlayerHUD()
// {
// 	const FName UIName = FName("ExampleHUD");
// 	OpHideUI(UIName);
// }

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