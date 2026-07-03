#include "Framework/Subsystem/KatanaLevelSubsystem.h"

#include "TimerManager.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Framework/KatanaSystemSettings.h"
#include "Framework/DataAsset/LevelDataAsset.h"
#include "Kismet/GameplayStatics.h"

void UKatanaLevelSubsystem::LoadLevel(const FName TargetLevelName)
{
	const UKatanaSystemSettings* SystemSettings = GetDefault<UKatanaSystemSettings>();
	if (!SystemSettings)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaLevelSubsystem : SystemSettings is null"));
		return;
	}

	const ULevelDataAsset* LevelDataAsset = SystemSettings->LevelDataAsset.LoadSynchronous();
	if (!LevelDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaLevelSubsystem : LevelDataAsset is null"));
		return;
	}

	if (!LevelDataAsset->LevelInfoMap.Contains(TargetLevelName))
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaLevelSubsystem : TargetLevelName is not found in LevelDataAsset"));
		return;
	}

	TargetMap = LevelDataAsset->LevelInfoMap[TargetLevelName].LevelMap;
	DelayLoadTime = 0.0f;
	MaxDelayLoadTime = LevelDataAsset->DelayLoadTime;
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetGameInstance(), LevelDataAsset->LoadingLevelInfo.LevelMap);
}

void UKatanaLevelSubsystem::StartLoadingTargetMapAsync()
{
	if (TargetMap.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaLevelSubsystem : TargetMap is null"));
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(LoopTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(LoopTimerHandle, this, &UKatanaLevelSubsystem::OnLoadingProgressTimer, LoopRate, true);

	LoadingHandle = StreamableManager.RequestAsyncLoad(
		TargetMap.ToSoftObjectPath()
	);
}

float UKatanaLevelSubsystem::GetLoadingProgress() const
{
	if (!LoadingHandle.IsValid())
		return 0.0f;

	const float TimeProgress = FMath::Clamp(DelayLoadTime / MaxDelayLoadTime, 0.0f, 1.0f);
	const float LoadingProgress = LoadingHandle->GetProgress();
	return FMath::Min(TimeProgress, LoadingProgress);
}

void UKatanaLevelSubsystem::OnLoadingProgressTimer()
{
	DelayLoadTime += LoopRate;

	UE_LOG(LogTemp, Warning, TEXT("UKatanaLevelSubsystem : LoadingProgress: %f"), GetLoadingProgress());

	OnLoadingProgressUpdated.Broadcast(GetLoadingProgress());

	if (GetLoadingProgress() < 1.0f)
		return;

	UE_LOG(LogTemp, Warning, TEXT("UKatanaLevelSubsystem : OpenLevelBySoftObjectPtr"));

	GetWorld()->GetTimerManager().ClearTimer(LoopTimerHandle);

	UGameplayStatics::OpenLevelBySoftObjectPtr(GetGameInstance(), TargetMap);

	LoadingHandle->ReleaseHandle();
	TargetMap.Reset();
}
