#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LoadingGameMode.generated.h"

/**
 *
 */
UCLASS()
class CH3_TEAM12_API ALoadingGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	ALoadingGameMode();
	virtual void BeginPlay() override;
};
