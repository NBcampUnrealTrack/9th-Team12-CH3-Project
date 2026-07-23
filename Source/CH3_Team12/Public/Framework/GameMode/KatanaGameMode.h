#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KatanaGameMode.generated.h"

class AEntranceWall;

DECLARE_MULTICAST_DELEGATE(FBossRoomEnteredDelegate);

/**
 *
 */
UCLASS()
class CH3_TEAM12_API AKatanaGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	FBossRoomEnteredDelegate OnBossRoomEntered;

	void SlowMotion(UWorld* InWorld, float InGamePlayRate, float InTime);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BGM")
	TObjectPtr<USoundBase> BGMSound;

	virtual void BeginPlay() override;

	void ChangeGamePlayRate(UWorld* InWorld, float InGamePlayRate);

private:
	AEntranceWall* FindEntranceWall() const;

	UFUNCTION()
	void HandleBossRoomEntered() const;
};
