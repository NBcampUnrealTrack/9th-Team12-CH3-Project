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


void UKatanaSoundManagerSubsystem::LoadAudioSettings(const UObject* WorldContextObject)
{
	UKatanaSoundManagerSubsystem* Subsystem = OpGet(WorldContextObject);
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("UKatanaSoundManagerSubsystem: Subsystem is null"));
		return;
	}

	Subsystem->OpLoadAudioSettings();
}

void UKatanaSoundManagerSubsystem::PlaySound2D(const UObject* WorldContextObject, EAudioType AudioType,
                                               USoundBase* SoundBase, float VolumeMultiplier, float PitchMultiplier)
{
	UKatanaSoundManagerSubsystem* Subsystem = OpGet(WorldContextObject);
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("UKatanaSoundManagerSubsystem: Subsystem is null"));

		UGameplayStatics::PlaySound2D(
			WorldContextObject,
			SoundBase,
			VolumeMultiplier,
			PitchMultiplier
		);
		return;
	}

	Subsystem->OpPlaySound2D(AudioType, SoundBase, VolumeMultiplier, PitchMultiplier);
}

void UKatanaSoundManagerSubsystem::PlaySoundAtLocation(const UObject* WorldContextObject, EAudioType AudioType,
                                                       USoundBase* SoundBase, const FVector& Location,
                                                       float VolumeMultiplier, float PitchMultiplier)
{
	UKatanaSoundManagerSubsystem* Subsystem = OpGet(WorldContextObject);
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("UKatanaSoundManagerSubsystem: Subsystem is null"));

		UGameplayStatics::PlaySoundAtLocation(
			WorldContextObject,
			SoundBase,
			Location,
			VolumeMultiplier,
			PitchMultiplier
		);
		return;
	}

	Subsystem->OpPlaySoundAtLocation(AudioType, SoundBase, Location, VolumeMultiplier, PitchMultiplier);
}

float UKatanaSoundManagerSubsystem::GetVolume(const UObject* WorldContextObject, EAudioType AudioType)
{
	UKatanaSoundManagerSubsystem* Subsystem = OpGet(WorldContextObject);
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("UKatanaSoundManagerSubsystem: Subsystem is null"));
		return 1.0f;
	}

	return Subsystem->OpGetVolume(AudioType);
}

void UKatanaSoundManagerSubsystem::SetVolume(const UObject* WorldContextObject, EAudioType AudioType, float NewVolume)
{
	UKatanaSoundManagerSubsystem* Subsystem = OpGet(WorldContextObject);
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("UKatanaSoundManagerSubsystem: Subsystem is null"));
		return;
	}

	Subsystem->OpSetVolume(AudioType, NewVolume);
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

	if (CachedSoundDataAsset->MasterControlBus)
		ControlBusMap.Add(EAudioType::Master, CachedSoundDataAsset->MasterControlBus);

	if (CachedSoundDataAsset->BGMControlBus)
		ControlBusMap.Add(EAudioType::BGM, CachedSoundDataAsset->BGMControlBus);

	if (CachedSoundDataAsset->SFXControlBus)
		ControlBusMap.Add(EAudioType::SFX, CachedSoundDataAsset->SFXControlBus);

	if (CachedSoundDataAsset->UIControlBus)
		ControlBusMap.Add(EAudioType::UI, CachedSoundDataAsset->UIControlBus);

	UE_LOG(LogTemp, Warning, TEXT("ControlBusMap: %d"), ControlBusMap.Num());
}

float UKatanaSoundManagerSubsystem::GetDefaultVolume(EAudioType AudioType)
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
	case EAudioType::UI:
		return DefaultSaveGame->UIVolume;
	default:
		return 0.0f;
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

UKatanaSoundManagerSubsystem* UKatanaSoundManagerSubsystem::OpGet(const UObject* WorldContextObject)
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

void UKatanaSoundManagerSubsystem::OpLoadAudioSettings()
{
	UE_LOG(LogTemp, Warning, TEXT("LoadAudioSettings"));

	TObjectPtr<UKatanaSoundSettingsSaveGame> LoadGameInstance = LoadSoundSettingsSaveGame(SlotName);

	if (!LoadGameInstance)
		LoadGameInstance = CreateDefaultSoundSettingsSaveGame();

	if (!LoadGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaSoundManagerSubsystem : LoadGameInstance is null"));
		return;
	}

	//LoadGameInstance->MasterVolume
	UE_LOG(LogTemp, Warning, TEXT("LoadGameInstance->MasterVolume : %f"), LoadGameInstance->MasterVolume);

	if (ControlBusMap.Contains(EAudioType::Master))
		UAudioModulationStatics::SetGlobalBusMixValue(GetWorld(), ControlBusMap[EAudioType::Master].Get(),
		                                              LoadGameInstance->MasterVolume, 0.0f);

	if (ControlBusMap.Contains(EAudioType::BGM))
		UAudioModulationStatics::SetGlobalBusMixValue(GetWorld(), ControlBusMap[EAudioType::BGM].Get(),
		                                              LoadGameInstance->BGMVolume, 0.0f);

	if (ControlBusMap.Contains(EAudioType::SFX))
		UAudioModulationStatics::SetGlobalBusMixValue(GetWorld(), ControlBusMap[EAudioType::SFX].Get(),
		                                              LoadGameInstance->SFXVolume, 0.0f);

	if (ControlBusMap.Contains(EAudioType::UI))
		UAudioModulationStatics::SetGlobalBusMixValue(GetWorld(), ControlBusMap[EAudioType::UI].Get(),
		                                              LoadGameInstance->UIVolume, 0.0f);
}

UAudioComponent* UKatanaSoundManagerSubsystem::OpPlaySound2D(EAudioType AudioType, USoundBase* SoundBase,
                                                           float VolumeMultiplier, float PitchMultiplier)
{
	if (!SoundBase)
	{
		UE_LOG(LogTemp, Warning, TEXT("SoundBase is nullptr"));
		return nullptr;
	}

	UAudioComponent* AudioComp = UGameplayStatics::SpawnSound2D(GetWorld(), SoundBase, VolumeMultiplier,
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

UAudioComponent* UKatanaSoundManagerSubsystem::OpPlaySoundAtLocation(EAudioType AudioType, USoundBase* SoundBase,
                                                                   const FVector& Location, float VolumeMultiplier,
                                                                   float PitchMultiplier,
                                                                   USoundAttenuation* AttenuationSettings)
{
	if (!SoundBase)
	{
		UE_LOG(LogTemp, Warning, TEXT("SoundBase is nullptr"));
		return nullptr;
	}

	UAudioComponent* AudioComp = UGameplayStatics::SpawnSoundAtLocation(
		GetWorld(), SoundBase, Location, FRotator::ZeroRotator, VolumeMultiplier, PitchMultiplier, 0.0f,
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

float UKatanaSoundManagerSubsystem::OpGetVolume(const EAudioType AudioType) const
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
	case EAudioType::UI:
		return LoadGameInstance->UIVolume;
	default:
		return 0.0f;
	}
}

void UKatanaSoundManagerSubsystem::OpSetVolume(const EAudioType AudioType, const float NewVolume)
{
	UE_LOG(LogTemp, Display, TEXT("SetVolume: AudioType: %d, NewVolume: %f"), static_cast<int32>(AudioType), NewVolume);

	if (!ControlBusMap.Contains(AudioType)) return;

	UAudioModulationStatics::SetGlobalBusMixValue(GetWorld(), ControlBusMap[AudioType].Get(), NewVolume, 0.0f);

	TObjectPtr<UKatanaSoundSettingsSaveGame> SaveGameInstance = LoadSoundSettingsSaveGame(SlotName);

	if (!SaveGameInstance)
		SaveGameInstance = CreateDefaultSoundSettingsSaveGame();

	if (!SaveGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaSoundManagerSubsystem : SaveGameInstance is null"));
		return;
	}

	if (AudioType == EAudioType::Master) SaveGameInstance->MasterVolume = NewVolume;
	else if (AudioType == EAudioType::BGM) SaveGameInstance->BGMVolume = NewVolume;
	else if (AudioType == EAudioType::SFX) SaveGameInstance->SFXVolume = NewVolume;
	else if (AudioType == EAudioType::UI) SaveGameInstance->UIVolume = NewVolume;

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, 0);
}