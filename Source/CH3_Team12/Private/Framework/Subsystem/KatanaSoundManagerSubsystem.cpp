#include "Framework/Subsystem/KatanaSoundManagerSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "AudioModulationStatics.h"
#include "SoundControlBus.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Framework/KatanaSoundSettingsSaveGame.h"
#include "Framework/KatanaSystemSettings.h"
#include "Framework/DataAsset/SoundDataAsset.h"

namespace
{
	UKatanaSoundSettingsSaveGame* CreateDefaultSoundSettingsSaveGame()
	{
		return Cast<UKatanaSoundSettingsSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UKatanaSoundSettingsSaveGame::StaticClass())
		);
	}

	UKatanaSoundSettingsSaveGame* LoadSoundSettingsSaveGame(const FString& SlotName)
	{
		return Cast<UKatanaSoundSettingsSaveGame>(
			UGameplayStatics::LoadGameFromSlot(SlotName, 0)
		);
	}
}

UKatanaSoundManagerSubsystem* UKatanaSoundManagerSubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;

	const UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaSoundManagerSubsystem: World is null"));
		return nullptr;
	}

	const UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaSoundManagerSubsystem: GameInstance is null"));
		return nullptr;
	}

	return GameInstance->GetSubsystem<UKatanaSoundManagerSubsystem>();
}

void UKatanaSoundManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UKatanaSystemSettings* SystemSettings = GetDefault<UKatanaSystemSettings>();
	if (!SystemSettings)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaSoundManagerSubsystem : SystemSettings is null"));
		return;
	}

	CachedSoundDataAsset = SystemSettings->SoundDataAsset.LoadSynchronous();
	if (!CachedSoundDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaSoundManagerSubsystem : SoundDataAsset is null"));
		return;
	}

	if (!CachedSoundDataAsset->MasterControlBus.IsNull())
	{
		USoundControlBus* MasterControlBus = CachedSoundDataAsset->MasterControlBus.LoadSynchronous();
		if (!MasterControlBus)
		{
			UE_LOG(LogTemp, Error, TEXT("UKatanaSoundManagerSubsystem : MasterControlBus is null"));
		}

		ControlBusMap.Add(EAudioType::Master, MasterControlBus);
	}

	if (!CachedSoundDataAsset->BGMControlBus.IsNull())
	{
		USoundControlBus* BGMControlBus = CachedSoundDataAsset->BGMControlBus.LoadSynchronous();
		if (!BGMControlBus)
		{
			UE_LOG(LogTemp, Error, TEXT("UKatanaSoundManagerSubsystem : BGMControlBus is null"));
		}

		ControlBusMap.Add(EAudioType::BGM, BGMControlBus);
	}

	if (!CachedSoundDataAsset->SFXControlBus.IsNull())
	{
		USoundControlBus* SFXControlBus = CachedSoundDataAsset->SFXControlBus.LoadSynchronous();
		if (!SFXControlBus)
		{
			UE_LOG(LogTemp, Error, TEXT("UKatanaSoundManagerSubsystem : SFXControlBus is null"));
		}

		ControlBusMap.Add(EAudioType::SFX, SFXControlBus);
	}

	UE_LOG(LogTemp, Warning, TEXT("ControlBusMap: %d"), ControlBusMap.Num());

	LoadAudioSettings();
}

float UKatanaSoundManagerSubsystem::GetDefaultVolume(EAudioType AudioType) const
{
	const UKatanaSoundSettingsSaveGame* DefaultSaveGame = CreateDefaultSoundSettingsSaveGame();

	if (!DefaultSaveGame)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaSoundManagerSubsystem : DefaultSaveGame is null"));
		return 0.0f;
	}

	switch (AudioType)
	{
	case EAudioType::Master:
		return DefaultSaveGame->MasterVolume;
	case EAudioType::BGM:
		return DefaultSaveGame->BGMVolume;
	case EAudioType::SFX:
		return DefaultSaveGame->SFXVolume;
	default:
		return 0.0f;
	}
}

float UKatanaSoundManagerSubsystem::GetVolume(const EAudioType AudioType)
{
	TObjectPtr<UKatanaSoundSettingsSaveGame> LoadGameInstance = LoadSoundSettingsSaveGame(SlotName);

	if (!LoadGameInstance)
	{
		LoadGameInstance = CreateDefaultSoundSettingsSaveGame();
	}

	switch (AudioType)
	{
	case EAudioType::Master:
		return LoadGameInstance->MasterVolume;
	case EAudioType::BGM:
		return LoadGameInstance->BGMVolume;
	case EAudioType::SFX:
		return LoadGameInstance->SFXVolume;
	default:
		return 0.0f;
	}
}

void UKatanaSoundManagerSubsystem::SetVolume(const EAudioType AudioType, const float NewVolume)
{
	UE_LOG(LogTemp, Display, TEXT("SetVolume: AudioType: %d, NewVolume: %f"), static_cast<int32>(AudioType), NewVolume);

	if (!ControlBusMap.Contains(AudioType)) return;

	UAudioModulationStatics::SetGlobalBusMixValue(GetWorld(), ControlBusMap[AudioType].Get(), NewVolume, 0.0f);

	TObjectPtr<UKatanaSoundSettingsSaveGame> SaveGameInstance = LoadSoundSettingsSaveGame(SlotName);

	if (!SaveGameInstance)
	{
		SaveGameInstance = CreateDefaultSoundSettingsSaveGame();
		// UE_LOG(LogTemp, Warning, TEXT("CreateGameInstance: %s"), *SaveGameInstance->GetName());
	}

	if (SaveGameInstance)
	{
		if (AudioType == EAudioType::Master) SaveGameInstance->MasterVolume = NewVolume;
		else if (AudioType == EAudioType::BGM) SaveGameInstance->BGMVolume = NewVolume;
		else if (AudioType == EAudioType::SFX) SaveGameInstance->SFXVolume = NewVolume;

		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, 0);
		// UE_LOG(LogTemp, Warning, TEXT("SaveGameInstance: %s"), *SaveGameInstance->GetName());
	}
}


UAudioComponent* UKatanaSoundManagerSubsystem::PlaySound2D(EAudioType AudioType, FString SoundKey,
                                                           float VolumeMultiplier, float PitchMultiplier)
{
	USoundBase* SoundToPlay = GetOrLoadSound(SoundKey);
	if (!SoundToPlay) return nullptr;

	UAudioComponent* AudioComp = UGameplayStatics::SpawnSound2D(GetWorld(), SoundToPlay, VolumeMultiplier,
	                                                            PitchMultiplier);

	if (AudioComp)
	{
		// 볼륨 설정에 필요한 사운드 모듈레이터 추가
		TSet<USoundModulatorBase*> TargetBusses;

		if (ControlBusMap.Contains(EAudioType::Master))
			TargetBusses.Add(Cast<USoundModulatorBase>(ControlBusMap[EAudioType::Master].Get()));

		if (ControlBusMap.Contains(AudioType))
			TargetBusses.Add(Cast<USoundModulatorBase>(ControlBusMap[AudioType].Get()));

		if (TargetBusses.Num() > 0)
			AudioComp->SetModulationRouting(TargetBusses, EModulationDestination::Volume, EModulationRouting::Override);
	}

	return AudioComp;
}

UAudioComponent* UKatanaSoundManagerSubsystem::PlaySound3DAtLocation(EAudioType AudioType, FString SoundKey,
                                                                     FVector Location,
                                                                     USoundAttenuation* AttenuationSettings)
{
	USoundBase* SoundToPlay = GetOrLoadSound(SoundKey);
	if (!SoundToPlay) return nullptr;

	UAudioComponent* AudioComp = UGameplayStatics::SpawnSoundAtLocation(
		GetWorld(), SoundToPlay, Location, FRotator::ZeroRotator, 1.0f, 1.0f, 0.0f,
		AttenuationSettings);

	if (AudioComp)
	{
		// 볼륨 설정에 필요한 사운드 모듈레이터 추가
		TSet<USoundModulatorBase*> TargetBusses;

		if (ControlBusMap.Contains(EAudioType::Master))
			TargetBusses.Add(Cast<USoundModulatorBase>(ControlBusMap[EAudioType::Master].Get()));

		if (ControlBusMap.Contains(AudioType))
			TargetBusses.Add(Cast<USoundModulatorBase>(ControlBusMap[AudioType].Get()));

		if (TargetBusses.Num() > 0)
			AudioComp->SetModulationRouting(TargetBusses, EModulationDestination::Volume, EModulationRouting::Override);
	}

	return AudioComp;
}

void UKatanaSoundManagerSubsystem::LoadAudioSettings()
{
	TObjectPtr<UKatanaSoundSettingsSaveGame> LoadGameInstance = LoadSoundSettingsSaveGame(SlotName);

	if (!LoadGameInstance)
	{
		LoadGameInstance = CreateDefaultSoundSettingsSaveGame();
	}

	if (LoadGameInstance)
	{
		if (ControlBusMap.Contains(EAudioType::Master))
			UAudioModulationStatics::SetGlobalBusMixValue(GetWorld(), ControlBusMap[EAudioType::Master].Get(),
			                                              LoadGameInstance->MasterVolume, 0.0f);

		if (ControlBusMap.Contains(EAudioType::BGM))
			UAudioModulationStatics::SetGlobalBusMixValue(GetWorld(), ControlBusMap[EAudioType::BGM].Get(),
			                                              LoadGameInstance->BGMVolume, 0.0f);

		if (ControlBusMap.Contains(EAudioType::SFX))
			UAudioModulationStatics::SetGlobalBusMixValue(GetWorld(), ControlBusMap[EAudioType::SFX].Get(),
			                                              LoadGameInstance->SFXVolume, 0.0f);
	}
}

USoundBase* UKatanaSoundManagerSubsystem::GetOrLoadSound(const FString& SoundKey) const
{
	if (!CachedSoundDataAsset || !CachedSoundDataAsset->SoundMap.Contains(SoundKey))
	{
		UE_LOG(LogTemp, Error, TEXT("KatanaSoundManagerSubsystem: 사운드 키값을 찾을 수 없습니다: %s"), *SoundKey);
		return nullptr;
	}

	TSoftObjectPtr<USoundBase> SoftSound = CachedSoundDataAsset->SoundMap[SoundKey];
	return SoftSound.IsPending() ? SoftSound.LoadSynchronous() : SoftSound.Get();
}
