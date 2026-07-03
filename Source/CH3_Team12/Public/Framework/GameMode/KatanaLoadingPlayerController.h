#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KatanaLoadingPlayerController.generated.h"

/**
 *
 */
UCLASS()
class CH3_TEAM12_API AKatanaLoadingPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
