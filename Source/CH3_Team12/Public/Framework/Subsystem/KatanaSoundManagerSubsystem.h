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
	SFX UMETA(DisplayName = "Sound Effects"),
	UI UMETA(DisplayName = "UI"),
};

UCLASS()
class CH3_TEAM12_API UKatanaSoundManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static void LoadAudioSettings(const UObject* WorldContextObject);

	static void PlaySound2D(const UObject* WorldContextObject, EAudioType AudioType, USoundBase* SoundBase,
	                        float VolumeMultiplier = 1.0f, float PitchMultiplier = 1.0f);

	static void PlaySoundAtLocation(const UObject* WorldContextObject, EAudioType AudioType, USoundBase* SoundBase,
	                                const FVector& Location, float VolumeMultiplier = 1.0f, float PitchMultiplier = 1.0f);

	static float GetDefaultVolume(EAudioType AudioType);
	static float GetVolume(const UObject* WorldContextObject, EAudioType AudioType);
	static void SetVolume(const UObject* WorldContextObject, EAudioType AudioType, float NewVolume);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

private:
	UPROPERTY()
	TObjectPtr<USoundDataAsset> CachedSoundDataAsset;

	UPROPERTY()
	TMap<EAudioType, TObjectPtr<USoundControlBus>> ControlBusMap;

	const FString SlotName = TEXT("AudioSettingsSlot_Modulation");

	USoundBase* GetOrLoadSound(const FString& SoundKey) const;

	static UKatanaSoundManagerSubsystem* OpGet(const UObject* WorldContextObject);

	void OpLoadAudioSettings();

	UAudioComponent* OpPlaySound2D(EAudioType AudioType, USoundBase* SoundBase, float VolumeMultiplier = 1.0f,
								 float PitchMultiplier = 1.0f);

	UAudioComponent* OpPlaySoundAtLocation(EAudioType AudioType, USoundBase* SoundBase, const FVector& Location,
	                                     float VolumeMultiplier = 1.0f, float PitchMultiplier = 1.0f,
	                                     USoundAttenuation* AttenuationSettings = nullptr);

	float OpGetVolume(EAudioType AudioType) const;
	void OpSetVolume(EAudioType AudioType, float NewVolume);
};
