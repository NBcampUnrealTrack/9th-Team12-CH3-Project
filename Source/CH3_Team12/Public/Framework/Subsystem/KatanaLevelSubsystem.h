#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Engine/TimerHandle.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KatanaLevelSubsystem.generated.h"

class AEntranceWall;
class AEnemyCharacterBase;
class USoundBase;
class ULevelDataAsset;
class UTexture2D;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingProgress, float, Progress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCompletedProgress);

/**
 *
 */
UCLASS()
class CH3_TEAM12_API UKatanaLevelSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UKatanaLevelSubsystem* Get(const UObject* WorldContextObject);
	static FName GetTargetMapName(const UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY()
	FOnLoadingProgress OnLoadingProgressUpdated;

	UPROPERTY()
	FOnCompletedProgress OnLoadingCompleted;

	UFUNCTION()
	void LoadLevel(FName TargetLevelName, float MinDelayLoadTime = 3.0f);

	UFUNCTION()
	UTexture2D* GetTargetMapTexture2D() const;

	UFUNCTION()
	TArray<FText> GetLoadingTipTexts() const;

	UFUNCTION()
	void StartLoadingTargetMapAsync();
	
	void RegisterBoss(AEnemyCharacterBase* Boss);
	void RegisterEntranceWall(AEntranceWall* EntranceWall);
	
	UFUNCTION()
	void StartBossBattle();
	
private:
	UPROPERTY()
	TObjectPtr<ULevelDataAsset> CachedLevelDataAsset;

	FName TargetMapName;

	const float LoopRate = 0.05f;

	FTimerHandle LoopTimerHandle;
	FTimerHandle DelayLoadTimerHandle;

	TSoftObjectPtr<UWorld> TargetMap;
	float DelayLoadTime;
	float MaxDelayLoadTime;

	FStreamableManager StreamableManager;
	TSharedPtr<FStreamableHandle> LoadingHandle;

	UPROPERTY()
	TObjectPtr<AEnemyCharacterBase> CurrentBoss;
	
	float GetLoadingProgress() const;
	void HandleLoadingProgressTimer();
	
	void ShowBossUI();
};
