#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "AudioSaveGame.generated.h"

/**
 *
 */
UCLASS()
class CH3_TEAM12_API UAudioSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UAudioSaveGame()
	{
		SaveSlotName = TEXT("AudioSettingsSlot_Modulation");
		UserIndex = 0;
		MasterVolume = 1.0f;
		BGMVolume = 0.8f;
		SFXVolume = 0.8f;
	}

	UPROPERTY()
	FString SaveSlotName;

	UPROPERTY()
	int32 UserIndex;

	// 사운드 분류별 볼륨 값 (0.0 ~ 1.0)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float MasterVolume = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float BGMVolume = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
	float SFXVolume = 0.8f;
};
