#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "KatanaSoundSettingsSaveGame.generated.h"

/**
 *
 */
UCLASS()
class CH3_TEAM12_API UKatanaSoundSettingsSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UKatanaSoundSettingsSaveGame();

	// 사운드 분류별 볼륨 값 (0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float MasterVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float BGMVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float SFXVolume;
};
