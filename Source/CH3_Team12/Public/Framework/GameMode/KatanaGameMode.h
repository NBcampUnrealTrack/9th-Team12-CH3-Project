#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KatanaGameMode.generated.h"

/**
 *
 */
UCLASS()
class CH3_TEAM12_API AKatanaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	void SlowMotion(UWorld* InWorld, float InGamePlayRate, float InTime);
protected:
	void ChangeGamePlayRate(UWorld* InWorld, float InGamePlayRate);
	
};
