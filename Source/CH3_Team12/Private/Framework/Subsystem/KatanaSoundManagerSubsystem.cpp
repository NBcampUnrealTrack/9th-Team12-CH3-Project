#include "Framework/Subsystem/KatanaSoundManagerSubsystem.h"

#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"
#include "AudioModulationStatics.h"
#include "SoundControlBus.h"
#include "Engine/World.h"
#include "Framework/AudioSaveGame.h"
#include "Framework/KatanaSystemSettings.h"
#include "Framework/DataAsset/SoundDataAsset.h"

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

	if (CachedSoundDataAsset->MasterControlBus.IsValid())
		ControlBusMap.Add(EAudioType::Master, CachedSoundDataAsset->MasterControlBus.Get());

	if (CachedSoundDataAsset->BGMControlBus.IsValid())
		ControlBusMap.Add(EAudioType::BGM, CachedSoundDataAsset->BGMControlBus.Get());

	if (CachedSoundDataAsset->SFXControlBus.IsValid())
		ControlBusMap.Add(EAudioType::SFX, CachedSoundDataAsset->SFXControlBus.Get());

	LoadAudioSettings();
}

USoundBase* UKatanaSoundManagerSubsystem::GetOrLoadSound(const FString& SoundKey) const
{
	if (!CachedSoundDataAsset || !CachedSoundDataAsset->SoundMap.Contains(SoundKey))
	{
		UE_LOG(LogTemp, Warning, TEXT("KatanaSoundManagerSubsystem: 사운드 키값을 찾을 수 없습니다: %s"), *SoundKey);
		return nullptr;
	}

	TSoftObjectPtr<USoundBase> SoftSound = CachedSoundDataAsset->SoundMap[SoundKey];
	return SoftSound.IsPending() ? SoftSound.LoadSynchronous() : SoftSound.Get();
}

UAudioComponent* UKatanaSoundManagerSubsystem::PlaySound2D(EAudioType AudioType, FString SoundKey,
                                                           float VolumeMultiplier,
                                                           float PitchMultiplier)
{
	USoundBase* SoundToPlay = GetOrLoadSound(SoundKey);
	if (!SoundToPlay) return nullptr;

	// 메타사운드가 켜질 때 내부 변수를 따로 세팅해줄 필요가 없으므로 정석대로 스폰만 합니다.
	UAudioComponent* AudioComp = UGameplayStatics::SpawnSound2D(GetWorld(), SoundToPlay, VolumeMultiplier,
	                                                            PitchMultiplier);

	if (AudioComp)
	{
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

	// 스폰 시점에 감쇄 에셋 포인터를 깔끔하게 밀어 넣습니다.
	UAudioComponent* AudioComp = UGameplayStatics::SpawnSoundAtLocation(
		GetWorld(), SoundToPlay, Location, FRotator::ZeroRotator, 1.0f, 1.0f, 0.0f,
		AttenuationSettings);

	if (AudioComp)
	{
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

void UKatanaSoundManagerSubsystem::SetVolume(const EAudioType AudioType, const float NewVolume)
{
	if (!ControlBusMap.Contains(AudioType)) return;

	UAudioModulationStatics::SetGlobalBusMixValue(GetWorld(), ControlBusMap[AudioType].Get(), NewVolume, 0.0f);

	// 하드디스크 세이브 파일 영구 저장 처리
	TObjectPtr<UAudioSaveGame> SaveGameInstance = Cast<UAudioSaveGame>(
		UGameplayStatics::LoadGameFromSlot(TEXT("AudioSettingsSlot_Modulation"), 0));

	if (!SaveGameInstance)
	{
		SaveGameInstance = Cast<UAudioSaveGame>(UGameplayStatics::CreateSaveGameObject(UAudioSaveGame::StaticClass()));
	}

	if (SaveGameInstance)
	{
		if (AudioType == EAudioType::Master) SaveGameInstance->MasterVolume = NewVolume;
		else if (AudioType == EAudioType::BGM) SaveGameInstance->BGMVolume = NewVolume;
		else if (AudioType == EAudioType::SFX) SaveGameInstance->SFXVolume = NewVolume;

		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
	}
}

void UKatanaSoundManagerSubsystem::LoadAudioSettings()
{
	TObjectPtr<UAudioSaveGame> LoadGameInstance = Cast<UAudioSaveGame>(
		UGameplayStatics::LoadGameFromSlot(TEXT("AudioSettingsSlot_Modulation"), 0));

	if (!LoadGameInstance)
	{
		LoadGameInstance = Cast<UAudioSaveGame>(UGameplayStatics::CreateSaveGameObject(UAudioSaveGame::StaticClass()));
	}

	if (LoadGameInstance)
	{
		// 로드 시점에 보관된 수치들을 전역 컨트롤 버스에 바인딩 오버라이드 시켜줍니다.
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
