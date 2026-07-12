#pragma once

#include "CoreMinimal.h"
#include "../DataAsset/UIDataAsset.h"
#include "Blueprint/UserWidget.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "UObject/ScriptInterface.h"
#include "KatanaUIManagerSubsystem.generated.h"

class UEnemyAttributeComponent;
class UInventoryWidget;
class USoundSettingsWidget;
class UKatanaSoundManagerSubsystem;
class UKatanaLevelSubsystem;
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
	static UKatanaUIManagerSubsystem* Get(const UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void ShowMainMenuWidget();
	void HideMainMenuWidget();

	void ShowMainMenuSettingsWidget();
	void HideMainMenuSettingsWidget();

	void ShowInGameMenuWidget();
	void HideInGameMenuWidget();
	bool HasInGameMenuWidget() const;

	void ShowLoadingWidget();
	void HideLoadingWidget();

	void ShowPlayerWidget(UPlayerAttributeComponent* InAttributeComponent);
	void HidePlayerWidget();

	void ShowEnemyWidget(UEnemyAttributeComponent* InAttributeComponent);
	void HideEnemyWidget();

	void RegisterInventoryWidget(UInventoryWidget* InWidget);
	void UnregisterInventoryWidget(UInventoryWidget* InWidget);

	void RegisterSoundSettingsWidget(USoundSettingsWidget* InWidget);
	void UnregisterSoundSettingsWidget(USoundSettingsWidget* InWidget);

private:
	const FName MainMenuWidgetName = FName("MainMenuWidget");
	const FName MainMenuSettingsWidgetName = FName("MainMenuSettingsWidget");
	const FName InGameMenuWidgetName = FName("InGameMenuWidget");

	const FName LoadingWidgetName = FName("LoadingWidget");

	const FName PlayerWidgetName = FName("PlayerWidget");
	const FName EnemyWidgetName = FName("EnemyWidget");

	const FName InventoryWidgetName = FName("InventoryWidget");
	const FName SoundSettingsWidgetName = FName("SoundSettingsWidget");

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
