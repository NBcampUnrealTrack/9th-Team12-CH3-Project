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

	// 서브시스템 시동 시 에셋 로드 및 기존 세이브 볼륨 동기화
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** 2D 사운드 재생 (BGM, UI 등 공간감 없는 메타사운드용) */
	UFUNCTION(BlueprintCallable, Category = "Audio|Play")
	UAudioComponent* PlaySound2D(EAudioType AudioType, FString SoundKey, float VolumeMultiplier = 1.0f,
	                             float PitchMultiplier = 1.0f);

	/** 3D 공간 사운드 재생 (발소리, 이펙트 등 월드 좌표 기반 메타사운드용) */
	UFUNCTION(BlueprintCallable, Category = "Audio|Play")
	UAudioComponent* PlaySound3DAtLocation(EAudioType AudioType, FString SoundKey, FVector Location,
	                                       USoundAttenuation* AttenuationSettings = nullptr);

	/** UI 슬라이더 연동용 - 전역 오디오 모듈레이션 버스 수치 조절 및 자동 동기화 */
	UFUNCTION(BlueprintCallable, Category = "Audio|Settings")
	void SetVolume(EAudioType AudioType, float NewVolume);

	/** 세이브된 볼륨 설정을 읽어와 복구 */
	UFUNCTION(BlueprintCallable, Category = "Audio|Settings")
	void LoadAudioSettings();

private:
	// 내부 멤버 변수 자리는 TObjectPtr을 유지하여 가비지 컬렉터(GC) 누수를 방지합니다.
	UPROPERTY()
	TObjectPtr<USoundDataAsset> CachedSoundDataAsset;

	UPROPERTY()
	TMap<EAudioType, TObjectPtr<USoundControlBus>> ControlBusMap;

	USoundBase* GetOrLoadSound(const FString& SoundKey) const;
};
