#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KatanaSoundManagerSubsystem.generated.h"

class USoundDataAsset;
class UAudioComponent;
class USoundAttenuation;
class USoundControlBus;
class USoundBase;

UENUM(BlueprintType)
enum class EAudioType : uint8
{
	Master  UMETA(DisplayName = "Master Volume"),
	BGM     UMETA(DisplayName = "Background Music"),
	SFX     UMETA(DisplayName = "Sound Effects")
};

UCLASS()
class CH3_TEAM12_API UKatanaSoundManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UKatanaSoundManagerSubsystem* Get(const UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Audio|Play")
	UAudioComponent* PlaySound2D(EAudioType AudioType, FString SoundKey, float VolumeMultiplier = 1.0f,
	                             float PitchMultiplier = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "Audio|Play")
	UAudioComponent* PlaySound3DAtLocation(EAudioType AudioType, FString SoundKey, FVector Location,
	                                       USoundAttenuation* AttenuationSettings = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Audio|Settings")
	void SetVolume(EAudioType AudioType, float NewVolume);

	UFUNCTION(BlueprintCallable, Category = "Audio|Settings")
	void LoadAudioSettings();

private:
	UPROPERTY()
	TObjectPtr<USoundDataAsset> CachedSoundDataAsset;

	UPROPERTY()
	TMap<EAudioType, TObjectPtr<USoundControlBus>> ControlBusMap;

	USoundBase* GetOrLoadSound(const FString& SoundKey) const;
};
