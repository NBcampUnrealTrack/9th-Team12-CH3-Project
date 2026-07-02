#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/TimerHandle.h"
#include "LevelDataAsset.generated.h"

struct FTimerHandle;

USTRUCT(BlueprintType)
struct FLevelInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> LevelMap;
};

/**
 *
 */
UCLASS()
class CH3_TEAM12_API ULevelDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, FLevelInfo> LevelInfoMap;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLevelInfo LoadingLevelInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DelayLoadTime = 5.0f;

private:
	FTimerHandle DelayLoadTimerHandle;
};
