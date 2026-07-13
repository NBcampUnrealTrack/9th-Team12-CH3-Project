#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Engine/TimerHandle.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KatanaLevelSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingProgress, float, Progress);

/**
 *
 */
UCLASS()
class CH3_TEAM12_API UKatanaLevelSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UKatanaLevelSubsystem* Get(const UObject* WorldContextObject);

	UPROPERTY(BlueprintAssignable)
	FOnLoadingProgress OnLoadingProgressUpdated;

	UFUNCTION(BlueprintCallable)
	void LoadLevel(FName TargetLevelName);

	UFUNCTION(BlueprintCallable)
	void StartLoadingTargetMapAsync();

private:
	const float LoopRate = 0.05f;

	FTimerHandle LoopTimerHandle;
	FTimerHandle DelayLoadTimerHandle;

	TSoftObjectPtr<UWorld> TargetMap;
	float DelayLoadTime;
	float MaxDelayLoadTime;

	FStreamableManager StreamableManager;
	TSharedPtr<FStreamableHandle> LoadingHandle;

	float GetLoadingProgress() const;
	void OnLoadingProgressTimer();
};
