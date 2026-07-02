#include "Framework/LoadingGameMode.h"

#include "Engine/GameInstance.h"
#include "Framework/KatanaLevelSubsystem.h"

ALoadingGameMode::ALoadingGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ALoadingGameMode::BeginPlay()
{
	Super::BeginPlay();

	UKatanaLevelSubsystem* LevelSubsystem = GetGameInstance()->GetSubsystem<UKatanaLevelSubsystem>();
	LevelSubsystem->StartLoadingTargetMapAsync();
}
