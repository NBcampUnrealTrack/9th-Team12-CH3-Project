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