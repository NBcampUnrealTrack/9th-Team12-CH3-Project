#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KatanaLoadingGameMode.generated.h"

/**
 *
 */
UCLASS()
class CH3_TEAM12_API AKatanaLoadingGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	AKatanaLoadingGameMode();
	virtual void BeginPlay() override;
};
