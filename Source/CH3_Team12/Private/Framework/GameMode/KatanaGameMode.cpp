#include "Framework/GameMode/KatanaGameMode.h"

#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "Entity/Environment/EntranceWall.h"
#include "Framework/Subsystem/KatanaSoundManagerSubsystem.h"

void AKatanaGameMode::BeginPlay()
{
	Super::BeginPlay();

	UKatanaSoundManagerSubsystem::LoadAudioSettings(this);

	if (AEntranceWall* EntranceWall = FindEntranceWall())
	{
		EntranceWall->OnBossRoomEntered.AddDynamic(this, &AKatanaGameMode::HandleBossRoomEntered);
	}
}

void AKatanaGameMode::ChangeGamePlayRate(UWorld* InWorld, float InGamePlayRate)
{
	if (InWorld == nullptr)
	{
		return;
	}

	UGameplayStatics::SetGlobalTimeDilation(InWorld, InGamePlayRate);
}

void AKatanaGameMode::SlowMotion(UWorld* InWorld, float InGamePlayRate, float InTime)
{
	if (InWorld == nullptr)
	{
		return;
	}

	ChangeGamePlayRate(InWorld, InGamePlayRate);

	FTimerHandle TimerHandle;
	InWorld->GetTimerManager().SetTimer(
		TimerHandle,
		[this, InWorld]()
		{
			ChangeGamePlayRate(InWorld, 1.0f);
		},
		InTime,
		false);
}

AEntranceWall* AKatanaGameMode::FindEntranceWall() const
{
	const UWorld* World = GetWorld();
	if (!World) return nullptr;

	for (TActorIterator<AEntranceWall> It(World); It; ++It)
	{
		AEntranceWall* FoundActor = *It;
		if (!FoundActor)
			continue;
		return FoundActor;
	}

	return nullptr;
}

void AKatanaGameMode::HandleBossRoomEntered() const
{
	UKatanaSoundManagerSubsystem::PlaySound2D(this, EAudioType::BGM, BGMSound);
	OnBossRoomEntered.Broadcast();
}
