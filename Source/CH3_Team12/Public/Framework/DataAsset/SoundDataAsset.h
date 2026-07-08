#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SoundDataAsset.generated.h"

class USoundControlBus;
class USoundBase;

/**
 *
 */
UCLASS()
class CH3_TEAM12_API USoundDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Audio")
	TSoftObjectPtr<USoundControlBus> MasterControlBus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Audio")
	TSoftObjectPtr<USoundControlBus> BGMControlBus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Audio")
	TSoftObjectPtr<USoundControlBus> SFXControlBus;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	TMap<FString, TSoftObjectPtr<USoundBase>> SoundMap;
};