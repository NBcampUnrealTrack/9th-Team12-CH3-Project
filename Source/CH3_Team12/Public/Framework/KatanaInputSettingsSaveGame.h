#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "KatanaInputSettingsSaveGame.generated.h"

struct FKey;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UKatanaInputSettingsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UKatanaInputSettingsSaveGame();

	UPROPERTY(EditAnywhere, Category = "Settings")
	TMap<FName, FKey> KeyBindings;
};
