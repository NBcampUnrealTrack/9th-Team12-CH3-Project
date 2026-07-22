#include "Framework/GameMode/KatanaLoadingGameMode.h"

#include "Engine/GameInstance.h"
#include "Framework/Subsystem/KatanaLevelSubsystem.h"
#include "Framework/Subsystem/KatanaSoundManagerSubsystem.h"

AKatanaLoadingGameMode::AKatanaLoadingGameMode()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AKatanaLoadingGameMode::BeginPlay()
{
	Super::BeginPlay();

	UKatanaSoundManagerSubsystem::LoadAudioSettings(this);
	UKatanaLevelSubsystem* LevelSubsystem = GetGameInstance()->GetSubsystem<UKatanaLevelSubsystem>();
	LevelSubsystem->StartLoadingTargetMapAsync();
}
