#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KatanaMainMenuPlayerController.generated.h"

/**
 *
 */
UCLASS()
class CH3_TEAM12_API AKatanaMainMenuPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
