#pragma once

#include "CoreMinimal.h"
#include "Engine/World.h"
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
	Master UMETA(DisplayName = "Master Volume"),
	BGM UMETA(DisplayName = "Background Music"),
	SFX UMETA(DisplayName = "Sound Effects")
};

UCLASS()
class CH3_TEAM12_API UKatanaSoundManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UKatanaSoundManagerSubsystem* Get(const UObject* WorldContextObject);

	static void PlaySound2D(const UObject* WorldContextObject, EAudioType AudioType, USoundBase* SoundBase,
	                        float VolumeMultiplier = 1.0f, float PitchMultiplier = 1.0f);

	static void PlaySoundAtLocation(const UObject* WorldContextObject, EAudioType AudioType, USoundBase* SoundBase,
	                                const FVector& Location, float VolumeMultiplier = 1.0f, float PitchMultiplier = 1.0f);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void LoadAudioSettings();

	float GetDefaultVolume(EAudioType AudioType) const;

	float GetVolume(EAudioType AudioType);
	void SetVolume(EAudioType AudioType, float NewVolume);

private:
	UPROPERTY()
	TObjectPtr<USoundDataAsset> CachedSoundDataAsset;

	UPROPERTY()
	TMap<EAudioType, TObjectPtr<USoundControlBus>> ControlBusMap;

	const FString SlotName = TEXT("AudioSettingsSlot_Modulation");

	void OnWorldInitialized(UWorld* World, const UWorld::InitializationValues IVS);

	USoundBase* GetOrLoadSound(const FString& SoundKey) const;

	UAudioComponent* PlaySound2D(EAudioType AudioType, USoundBase* SoundBase, float VolumeMultiplier = 1.0f,
								 float PitchMultiplier = 1.0f);

	UAudioComponent* PlaySoundAtLocation(EAudioType AudioType, USoundBase* SoundBase, const FVector& Location,
	                                     float VolumeMultiplier = 1.0f, float PitchMultiplier = 1.0f,
	                                     USoundAttenuation* AttenuationSettings = nullptr);
};
