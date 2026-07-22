#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KatanaMainMenuGameMode.generated.h"

/**
 *
 */
UCLASS()
class CH3_TEAM12_API AKatanaMainMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BGM")
	TObjectPtr<USoundBase> BGMSound;

	virtual void BeginPlay() override;
};
