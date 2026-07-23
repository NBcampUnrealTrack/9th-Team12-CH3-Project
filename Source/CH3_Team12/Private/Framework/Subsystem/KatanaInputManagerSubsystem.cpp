#include "Framework/Subsystem/KatanaInputManagerSubsystem.h"

#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Framework/KatanaInputSettingsSaveGame.h"
#include "UserSettings/EnhancedInputUserSettings.h"

namespace
{
	UKatanaInputSettingsSaveGame* CreateDefaultInputSettingsSaveGame()
	{
		return Cast<UKatanaInputSettingsSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UKatanaInputSettingsSaveGame::StaticClass())
		);
	}

	UKatanaInputSettingsSaveGame* LoadInputSettingsSaveGame(const FString& SlotName)
	{
		return Cast<UKatanaInputSettingsSaveGame>(
			UGameplayStatics::LoadGameFromSlot(SlotName, 0)
		);
	}
}

UKatanaInputManagerSubsystem* UKatanaInputManagerSubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;

	const UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaInputManagerSubsystem: World is null"));
		return nullptr;
	}

	const UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaInputManagerSubsystem: GameInstance is null"));
		return nullptr;
	}

	return GameInstance->GetSubsystem<UKatanaInputManagerSubsystem>();
}

void UKatanaInputManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadInputSettings();
}

void UKatanaInputManagerSubsystem::LoadInputSettings()
{
	CurrentKeyBindings.Reset();

	const UKatanaInputSettingsSaveGame* SaveGameInstance = LoadInputSettingsSaveGame(SlotName);

	if (!SaveGameInstance)
	{
		SaveGameInstance = CreateDefaultInputSettingsSaveGame();
	}

	if (!SaveGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("KatanaInputManagerSubsystem: InputSettingsSaveGame 생성 실패"));
		return;
	}

	CurrentKeyBindings = SaveGameInstance->KeyBindings;
}

void UKatanaInputManagerSubsystem::ApplyAndSaveInputSettings()
{
	if (CurrentKeyBindings.IsEmpty())
	{
		CurrentKeyBindings = GetDefaultKeyBindings();
	}

	TObjectPtr<UKatanaInputSettingsSaveGame> SaveGameInstance = LoadInputSettingsSaveGame(SlotName);

	if (!SaveGameInstance)
	{
		SaveGameInstance = CreateDefaultInputSettingsSaveGame();
	}

	if (SaveGameInstance)
	{
		SaveGameInstance->KeyBindings = CurrentKeyBindings;
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, 0);
		UE_LOG(LogTemp, Log, TEXT("KatanaInputManagerSubsystem: 키 설정이 저장되었습니다."));
	}

	const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
		return;

	const ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer)
		return;

	const UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>();

	if (!InputSubsystem)
		return;

	UEnhancedInputUserSettings* UserSettings = InputSubsystem->GetUserSettings();
	if (!UserSettings)
		return;

	for (const auto& Elem : CurrentKeyBindings)
	{
		const FName MappingName = Elem.Key;
		const FKey NewKey = Elem.Value;

		FMapPlayerKeyArgs Args;
		Args.MappingName = MappingName;
		Args.NewKey = NewKey;
		Args.Slot = EPlayerMappableKeySlot::First;

		FGameplayTagContainer FailureReason;
		UserSettings->MapPlayerKey(Args, FailureReason);
	}

	UserSettings->ApplySettings();
	UserSettings->SaveSettings();

	UE_LOG(LogTemp, Log, TEXT("KatanaInputManagerSubsystem: Enhanced Input 설정이 갱신되었습니다."));
}

TMap<FName, FKey> UKatanaInputManagerSubsystem::GetDefaultKeyBindings() const
{
	const UKatanaInputSettingsSaveGame* DefaultSaveGame = CreateDefaultInputSettingsSaveGame();

	if (!DefaultSaveGame)
	{
		UE_LOG(LogTemp, Error, TEXT("KatanaInputManagerSubsystem: Default InputSettingsSaveGame 생성 실패"));
		return {};
	}

	return DefaultSaveGame->KeyBindings;
}

TMap<FName, FKey> UKatanaInputManagerSubsystem::GetCurrentKeyBindings() const
{
	return CurrentKeyBindings;
}

void UKatanaInputManagerSubsystem::SetKeyBinding(const FName ActionName, const FKey NewKey)
{
	CurrentKeyBindings.Add(ActionName, NewKey);
	// UE_LOG(LogTemp, Warning, TEXT("Key Bound -> Action: %s, Key: %s"), *ActionName.ToString(), *NewKey.ToString());
}
