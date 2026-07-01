#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UObject/SoftObjectPtr.h"
#include "KatanaSystemSettings.generated.h"

class UUIDataAsset;
class ULevelDataAsset;
/**
 *
 */
UCLASS(config=Game, defaultconfig, meta=(DisplayName="Katana System Settings"))
class CH3_TEAM12_API UKatanaSystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, config, Category="Level")
	TSoftObjectPtr<ULevelDataAsset> LevelDataAsset;

	UPROPERTY(EditAnywhere, config, Category="UI")
	TSoftObjectPtr<UUIDataAsset> UIDataAsset;
};
