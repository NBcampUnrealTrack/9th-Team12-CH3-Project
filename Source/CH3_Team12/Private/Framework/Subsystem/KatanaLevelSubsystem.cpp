#include "Framework/Subsystem/KatanaLevelSubsystem.h"

#include "EngineUtils.h"
#include "TimerManager.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Framework/KatanaSystemSettings.h"
#include "Framework/DataAsset/LevelDataAsset.h"
#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"

UKatanaLevelSubsystem* UKatanaLevelSubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return nullptr;

	const UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaLevelSubsystem: World is null"));
		return nullptr;
	}

	const UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaLevelSubsystem: GameInstance is null"));
		return nullptr;
	}

	return GameInstance->GetSubsystem<UKatanaLevelSubsystem>();
}

FName UKatanaLevelSubsystem::GetTargetMapName(const UObject* WorldContextObject)
{
	const UKatanaLevelSubsystem* KatanaLevelSubsystem = Get(WorldContextObject);
	if (!KatanaLevelSubsystem)
		return FName();

	return KatanaLevelSubsystem->TargetMapName;
}

void UKatanaLevelSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	const UKatanaSystemSettings* SystemSettings = GetDefault<UKatanaSystemSettings>();
	if (!SystemSettings)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaLevelSubsystem : SystemSettings is null"));
		return;
	}

	CachedLevelDataAsset = SystemSettings->LevelDataAsset.LoadSynchronous();
	if (!CachedLevelDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaLevelSubsystem : LevelDataAsset is null"));
	}
}

void UKatanaLevelSubsystem::LoadLevel(const FName TargetLevelName, const float MinDelayLoadTime)
{
	if (!CachedLevelDataAsset->LevelInfoMap.Contains(TargetLevelName))
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaLevelSubsystem : TargetLevelName is not found in LevelDataAsset"));
		return;
	}

	TargetMap = CachedLevelDataAsset->LevelInfoMap[TargetLevelName].LevelMap;
	TargetMapName = TargetLevelName;
	DelayLoadTime = 0.0f;
	MaxDelayLoadTime = FMath::Max(0.0f, MinDelayLoadTime);
	UGameplayStatics::OpenLevelBySoftObjectPtr(GetGameInstance(), CachedLevelDataAsset->LoadingLevelInfo.LevelMap);
}

UTexture2D* UKatanaLevelSubsystem::GetTargetMapTexture2D() const
{
	if (TargetMapName == TEXT(""))
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaLevelSubsystem : TargetMapName is null"));
		return nullptr;
	}

	if (!CachedLevelDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaLevelSubsystem : CachedLevelDataAsset is null"));
		return nullptr;
	}

	if (!CachedLevelDataAsset->LevelInfoMap.Contains(TargetMapName))
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaLevelSubsystem : TargetMapName is not found in LevelDataAsset"));
		return nullptr;
	}

	return CachedLevelDataAsset->LevelInfoMap[TargetMapName].LoadingLevelTexture.LoadSynchronous();
}

TArray<FText> UKatanaLevelSubsystem::GetLoadingTipTexts() const
{
	if (!CachedLevelDataAsset)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaLevelSubsystem : CachedLevelDataAsset is null"));
		return {};
	}

	return CachedLevelDataAsset->LoadingTipTexts;
}

// LoadingGameMode 에서 실행하는 부분
void UKatanaLevelSubsystem::StartLoadingTargetMapAsync()
{
	if (TargetMap.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaLevelSubsystem : TargetMap is null"));
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(LoopTimerHandle);
	GetWorld()->GetTimerManager().SetTimer(LoopTimerHandle, this, &UKatanaLevelSubsystem::HandleLoadingProgressTimer,
	                                       LoopRate, true);

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

void UKatanaLevelSubsystem::HandleLoadingProgressTimer()
{
	DelayLoadTime += LoopRate;

	OnLoadingProgressUpdated.Broadcast(GetLoadingProgress());

	if (GetLoadingProgress() < 1.0f)
		return;

	GetWorld()->GetTimerManager().ClearTimer(LoopTimerHandle);

	UGameplayStatics::OpenLevelBySoftObjectPtr(GetGameInstance(), TargetMap);

	LoadingHandle->ReleaseHandle();
	TargetMap.Reset();

	OnLoadingCompleted.Broadcast();
}
