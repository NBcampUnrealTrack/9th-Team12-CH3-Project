#pragma once

#include "CoreMinimal.h"
#include "Engine/StreamableManager.h"
#include "Engine/TimerHandle.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KatanaLevelSubsystem.generated.h"

/**
 *
 */
UCLASS()
class CH3_TEAM12_API UKatanaLevelSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void LoadLevel(FName TargetLevelName);

	UFUNCTION(BlueprintCallable)
	void StartLoadingTargetMapAsync();

	float GetLoadingProgress() const;

private:
	const float LoopRate = 0.1f;

	FTimerHandle LoopTimerHandle;
	FTimerHandle DelayLoadTimerHandle;

	TSoftObjectPtr<UWorld> TargetMap;
	float DelayLoadTime;
	float MaxDelayLoadTime;

	FStreamableManager StreamableManager;
	TSharedPtr<FStreamableHandle> LoadingHandle;

	void OnLoadingProgressTimer();
};
