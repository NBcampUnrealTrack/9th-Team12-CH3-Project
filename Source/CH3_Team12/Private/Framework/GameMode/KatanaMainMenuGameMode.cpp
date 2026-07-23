#include "Framework/GameMode/KatanaMainMenuGameMode.h"

#include "Framework/Subsystem/KatanaSoundManagerSubsystem.h"

void AKatanaMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	UKatanaSoundManagerSubsystem::LoadAudioSettings(this);
	UKatanaSoundManagerSubsystem::PlaySound2D(this, EAudioType::BGM, BGMSound);
}
