#pragma once

#include "CoreMinimal.h"
#include "UIDataAsset.h"
#include "Blueprint/UserWidget.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UObject/ScriptInterface.h"
#include "KatanaUIManagerSubsystem.generated.h"

class UPlayerAttributeComponent;
class IPresenterInterface;
class UUIDataAsset;
class UUserWidget;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UKatanaUIManagerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void ShowPlayerWidget(UPlayerAttributeComponent* InAttributeComponent);
	void HidePlayerWidget();

private:
	UPROPERTY()
	TSoftObjectPtr<UUIDataAsset> UIDataAsset = nullptr;

	UPROPERTY()
	TMap<FName, UUserWidget*> ActiveViews;

	UPROPERTY()
	TMap<FName, TScriptInterface<IPresenterInterface>> ActivePresenters;

	template <typename T>
	T* OpShowUI(const FName UIName)
	{
		if (ActiveViews.Contains(UIName))
			return Cast<T>(ActiveViews[UIName]);

		if (UIDataAsset.IsNull())
		{
			UE_LOG(LogTemp, Error, TEXT("UKatanaUIManagerSubsystem: UIDataAsset is null"));
			return nullptr;
		}

		if (!UIDataAsset->WidgetInfoMap.Contains(UIName))
		{
			UE_LOG(LogTemp, Error, TEXT("UKatanaUIManagerSubsystem: WidgetInfoMap does not contain %s"), *UIName.ToString());
			return nullptr;
		}

		const FUIWidgetInfo Info = UIDataAsset->WidgetInfoMap[UIName];
		if (!Info.WidgetClass)
		{
			UE_LOG(LogTemp, Error, TEXT("UKatanaUIManagerSubsystem: %s WidgetClass is null"), *UIName.ToString());
			return nullptr;
		}

		T* NewWidget = CreateWidget<T>(GetWorld(), Info.WidgetClass);
		if (NewWidget)
		{
			NewWidget->AddToViewport(Info.ZOrder);
			ActiveViews.Add(UIName, NewWidget);
		}

		return NewWidget;
	}

	void OpHideUI(const FName UIName);
};
