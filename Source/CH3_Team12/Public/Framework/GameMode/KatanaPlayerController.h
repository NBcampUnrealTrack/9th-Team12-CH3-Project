#pragma once

#include "CoreMinimal.h"
#include "Entity/Player/PlayerControllerBase.h"
#include "KatanaPlayerController.generated.h"

class UPlayerAttributeComponent;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API AKatanaPlayerController : public APlayerControllerBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

// public:
// 	virtual void Tick(float DeltaSeconds) override;
//
// private:
// 	float Time = 0.0f;
// 	float SetTime = 5.0f;
//
// 	UPlayerAttributeComponent* AttributeComponentTest = nullptr;
};
