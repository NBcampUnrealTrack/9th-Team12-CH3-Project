#include "Framework/LevelTestGameMode.h"

#include "Engine/GameInstance.h"
#include "Framework/KatanaLevelSubsystem.h"

void ALevelTestGameMode::BeginPlay()
{
	UKatanaLevelSubsystem* KatanaLevelSubsystem = GetGameInstance()->GetSubsystem<UKatanaLevelSubsystem>();
	KatanaLevelSubsystem->LoadLevel("GameLevel");
}
