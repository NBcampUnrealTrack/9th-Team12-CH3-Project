#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"

#include "Entity/Enemy/Component/EnemyAttributeComponent.h"
#include "Framework/KatanaSystemSettings.h"
#include "Framework/PresenterInterface.h"
#include "UI/InGameMenuPresenter.h"
#include "UI/InGameMenuWidget.h"
#include "UI/InventoryPresenter.h"
#include "UI/LoadingPresenter.h"
#include "UI/LoadingWidget.h"
#include "UI/MainMenuPresenter.h"
#include "UI/MainMenuWidget.h"
#include "UI/HUD/EnemyPresenter.h"
#include "UI/HUD/EnemyWidget.h"
#include "UI/HUD/PlayerPresenter.h"
#include "UI/HUD/PlayerWidget.h"
#include "UI/Settings/GraphicSettingsPresenter.h"
#include "UI/Settings/InputSettingsPresenter.h"
#include "UI/Settings/MainMenuSettingsPresenter.h"
#include "UI/Settings/MainMenuSettingsWidget.h"
#include "UI/Settings/SoundSettingsPresenter.h"

UKatanaUIManagerSubsystem* UKatanaUIManagerSubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;

	const UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaUIManagerSubsystem: World is null"));
		return nullptr;
	}

	//UGameplayStatics::GetPlayerController(GetWorld(), 0);
	const ULocalPlayer* LocalPlayer = World->GetFirstLocalPlayerFromController();
	if (!LocalPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaUIManagerSubsystem: LocalPlayer is null"));
		return nullptr;
	}

	return LocalPlayer->GetSubsystem<UKatanaUIManagerSubsystem>();
}

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
	const FName WidgetName = MainMenuWidgetName;
	UMainMenuWidget* ActiveView = OpShowUI<UMainMenuWidget>(WidgetName);
	if (!ActiveView)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Widget 를 생성하지 못했습니다."), *WidgetName.ToString());
		return;
	}

	UMainMenuPresenter* NewPresenter = NewObject<UMainMenuPresenter>(this);
	if (!NewPresenter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Presenter 를 생성하지 못했습니다."), *WidgetName.ToString());
		return;
	}

	NewPresenter->Initialize(ActiveView);
	ActivePresenters.Add(WidgetName, NewPresenter);
}

void UKatanaUIManagerSubsystem::HideMainMenuWidget()
{
	OpHideUI(MainMenuWidgetName);
}

void UKatanaUIManagerSubsystem::ShowMainMenuSettingsWidget()
{
	const FName WidgetName = MainMenuSettingsWidgetName;
	UMainMenuSettingsWidget* ActiveView = OpShowUI<UMainMenuSettingsWidget>(WidgetName);
	if (!ActiveView)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Widget 를 생성하지 못했습니다."), *WidgetName.ToString());
		return;
	}

	UMainMenuSettingsPresenter* NewPresenter = NewObject<UMainMenuSettingsPresenter>(this);
	if (!NewPresenter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Presenter 를 생성하지 못했습니다."), *WidgetName.ToString());
		return;
	}

	NewPresenter->Initialize(ActiveView);
	ActivePresenters.Add(WidgetName, NewPresenter);
}

void UKatanaUIManagerSubsystem::HideMainMenuSettingsWidget()
{
	OpHideUI(MainMenuSettingsWidgetName);
}

void UKatanaUIManagerSubsystem::ShowInGameMenuWidget()
{
	const FName WidgetName = InGameMenuWidgetName;
	UInGameMenuWidget* ActiveView = OpShowUI<UInGameMenuWidget>(WidgetName);
	if (!ActiveView)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Widget 를 생성하지 못했습니다."), *WidgetName.ToString());
		return;
	}

	UInGameMenuPresenter* NewPresenter = NewObject<UInGameMenuPresenter>(this);
	if (!NewPresenter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Presenter 를 생성하지 못했습니다."), *WidgetName.ToString());
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("%s 생성."), *WidgetName.ToString());

	NewPresenter->Initialize(ActiveView);
	ActivePresenters.Add(WidgetName, NewPresenter);
}

void UKatanaUIManagerSubsystem::HideInGameMenuWidget()
{
	OpHideUI(InGameMenuWidgetName);
}

bool UKatanaUIManagerSubsystem::HasInGameMenuWidget() const
{
	return ActiveViews.Contains(InGameMenuWidgetName) && ActivePresenters.Contains(InGameMenuWidgetName);
}

void UKatanaUIManagerSubsystem::ShowLoadingWidget()
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

	NewPresenter->Initialize(ActiveView);
	ActivePresenters.Add(LoadingWidgetName, NewPresenter);
}

void UKatanaUIManagerSubsystem::HideLoadingWidget()
{
	OpHideUI(LoadingWidgetName);
}

void UKatanaUIManagerSubsystem::ShowPlayerWidget(UPlayerAttributeComponent* InAttributeComponent)
{
	const FName WidgetName = PlayerWidgetName;
	UPlayerWidget* ActiveView = OpShowUI<UPlayerWidget>(WidgetName);
	if (!ActiveView) return;

	UPlayerPresenter* NewPresenter = NewObject<UPlayerPresenter>(this);
	if (!NewPresenter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Presenter 를 생성하지 못했습니다."), *WidgetName.ToString());
		return;
	}

	NewPresenter->Initialize(InAttributeComponent, ActiveView);
	ActivePresenters.Add(WidgetName, NewPresenter);
}

void UKatanaUIManagerSubsystem::HidePlayerWidget()
{
	OpHideUI(PlayerWidgetName);
}

void UKatanaUIManagerSubsystem::ShowEnemyWidget(const FString& InName, UEnemyAttributeComponent* InAttributeComponent)
{
	const FName WidgetName = EnemyWidgetName;
	UEnemyWidget* ActiveView = OpShowUI<UEnemyWidget>(WidgetName);
	if (!ActiveView) return;

	UEnemyPresenter* NewPresenter = NewObject<UEnemyPresenter>(this);
	if (!NewPresenter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Presenter 를 생성하지 못했습니다."), *WidgetName.ToString());
		return;
	}

	NewPresenter->Initialize(InName, InAttributeComponent, ActiveView);
	ActivePresenters.Add(WidgetName, NewPresenter);
}

void UKatanaUIManagerSubsystem::HideEnemyWidget()
{
	OpHideUI(EnemyWidgetName);
}

void UKatanaUIManagerSubsystem::RegisterInventoryWidget(UInventoryWidget* InWidget)
{
	const FName WidgetName = InventoryWidgetName;
	if (!InWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Widget 이 유효하지 않습니다."), *WidgetName.ToString());
		return;
	}

	UInventoryPresenter* NewPresenter = NewObject<UInventoryPresenter>(this);
	if (!NewPresenter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Presenter 를 생성하지 못했습니다."), *WidgetName.ToString());
		return;
	}

	NewPresenter->Initialize(InWidget);
	ActivePresenters.Add(WidgetName, NewPresenter);
}

void UKatanaUIManagerSubsystem::UnregisterInventoryWidget(UInventoryWidget* InWidget)
{
	OpHideUI(InventoryWidgetName);
}

void UKatanaUIManagerSubsystem::RegisterSoundSettingsWidget(USoundSettingsWidget* InWidget)
{
	const FName WidgetName = SoundSettingsWidgetName;
	if (!InWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Widget 이 유효하지 않습니다."), *WidgetName.ToString());
		return;
	}

	USoundSettingsPresenter* NewPresenter = NewObject<USoundSettingsPresenter>(this);
	if (!NewPresenter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Presenter 를 생성하지 못했습니다."), *WidgetName.ToString());
		return;
	}

	NewPresenter->Initialize(InWidget);
	ActivePresenters.Add(WidgetName, NewPresenter);
}

void UKatanaUIManagerSubsystem::UnregisterSoundSettingsWidget(USoundSettingsWidget* InWidget)
{
	OpHideUI(SoundSettingsWidgetName);
}

void UKatanaUIManagerSubsystem::RegisterGraphicSettingsWidget(UGraphicSettingsWidget* InWidget)
{
	const FName WidgetName = GraphicSettingsWidgetName;
	if (!InWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Widget 이 유효하지 않습니다."), *WidgetName.ToString());
		return;
	}

	UGraphicSettingsPresenter* NewPresenter = NewObject<UGraphicSettingsPresenter>(this);
	if (!NewPresenter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Presenter 를 생성하지 못했습니다."), *WidgetName.ToString());
		return;
	}

	NewPresenter->Initialize(InWidget);
	ActivePresenters.Add(WidgetName, NewPresenter);
}

void UKatanaUIManagerSubsystem::UnregisterGraphicSettingsWidget(UGraphicSettingsWidget* InWidget)
{
	OpHideUI(GraphicSettingsWidgetName);
}

void UKatanaUIManagerSubsystem::RegisterInputSettingsWidget(UInputSettingsWidget* InWidget)
{
	const FName WidgetName = InputSettingsWidgetName;
	if (!InWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Widget 이 유효하지 않습니다."), *WidgetName.ToString());
		return;
	}

	UInputSettingsPresenter* NewPresenter = NewObject<UInputSettingsPresenter>(this);
	if (!NewPresenter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Presenter 를 생성하지 못했습니다."), *WidgetName.ToString());
		return;
	}

	NewPresenter->Initialize(InWidget);
	ActivePresenters.Add(WidgetName, NewPresenter);
}

void UKatanaUIManagerSubsystem::UnregisterInputSettingsWidget(UInputSettingsWidget* InWidget)
{
	OpHideUI(InputSettingsWidgetName);
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
