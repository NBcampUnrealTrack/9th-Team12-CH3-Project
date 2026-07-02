#pragma once

#include "CoreMinimal.h"
#include "LevelDataAsset.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/SoftObjectPtr.h"
#include "KatanaSystemSettings.generated.h"

class UUIDataAsset;
/**
 *
 */
UCLASS(config=Game, DefaultConfig, meta=(DisplayName="Katana System Settings"))
class CH3_TEAM12_API UKatanaSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category="Level")
	TSoftObjectPtr<ULevelDataAsset> LevelDataAsset;

	UPROPERTY(EditAnywhere, Config, Category="UI")
	TSoftObjectPtr<UUIDataAsset> UIDataAsset;
};
