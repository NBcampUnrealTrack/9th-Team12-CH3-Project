#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KatanaInputManagerSubsystem.generated.h"

UCLASS()
class CH3_TEAM12_API UKatanaInputManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UKatanaInputManagerSubsystem* Get(const UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void ApplyAndSaveInputSettings();
	void LoadInputSettings();

	TMap<FName, FKey> GetDefaultKeyBindings() const;
	TMap<FName, FKey> GetCurrentKeyBindings() const;

	void SetKeyBinding(FName ActionName, FKey NewKey);

private:
	const FString SlotName = TEXT("InputSettingsSlot");

	UPROPERTY()
	TMap<FName, FKey> CurrentKeyBindings;
};
