#include "Framework/GameMode/LoadingGameMode.h"

#include "Engine/GameInstance.h"
#include "Framework/Subsystem/KatanaLevelSubsystem.h"

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
