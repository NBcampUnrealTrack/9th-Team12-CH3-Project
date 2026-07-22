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
	float MasterVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float BGMVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float SFXVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float UIVolume = 1.0f;
};
