#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "KatanaStageRecordSaveGame.generated.h"

UCLASS()
class CH3_TEAM12_API UKatanaStageRecordSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stage Record")
	TMap<FName, float> BestStageTimeMap;
};