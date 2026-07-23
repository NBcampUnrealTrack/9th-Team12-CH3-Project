#include "Framework/Subsystem/KatanaStageRecordManagerSubsystem.h"

#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Framework/KatanaStageRecordSaveGame.h"
#include "Kismet/GameplayStatics.h"

namespace
{
	UKatanaStageRecordSaveGame* CreateDefaultStageRecordSaveGame()
	{
		return Cast<UKatanaStageRecordSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UKatanaStageRecordSaveGame::StaticClass())
		);
	}

	UKatanaStageRecordSaveGame* LoadStageRecordSaveGame(const FString& SlotName)
	{
		return Cast<UKatanaStageRecordSaveGame>(
			UGameplayStatics::LoadGameFromSlot(SlotName, 0)
		);
	}
}

UKatanaStageRecordManagerSubsystem* UKatanaStageRecordManagerSubsystem::Get(const UObject* WorldContextObject)
{
	if (!WorldContextObject)
		return nullptr;

	const UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaStageRecordManagerSubsystem: World is null"));
		return nullptr;
	}

	const UGameInstance* GameInstance = World->GetGameInstance();
	if (!GameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("UKatanaStageRecordManagerSubsystem: GameInstance is null"));
		return nullptr;
	}

	return GameInstance->GetSubsystem<UKatanaStageRecordManagerSubsystem>();
}

void UKatanaStageRecordManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	LoadStageRecords();
}

void UKatanaStageRecordManagerSubsystem::StartStageRecord(FName StageName)
{
	if (StageName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("KatanaStageRecordManagerSubsystem: StageName is None"));
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("KatanaStageRecordManagerSubsystem: World is null"));
		return;
	}

	CurrentRecordingStageName = StageName;
	RecordStartTime = World->GetTimeSeconds();
	bIsRecording = true;

	FinishStageTime = 0.0f;

	UE_LOG(LogTemp, Log, TEXT("KatanaStageRecordManagerSubsystem: 스테이지 기록 시작 - %s"), *StageName.ToString());
}

float UKatanaStageRecordManagerSubsystem::FinishStageRecord(FName StageName)
{
	if (!bIsRecording)
	{
		UE_LOG(LogTemp, Warning, TEXT("KatanaStageRecordManagerSubsystem: 현재 기록 중이 아닙니다."));
		return 0.0f;
	}

	if (StageName.IsNone())
	{
		StageName = CurrentRecordingStageName;
	}

	if (StageName != CurrentRecordingStageName)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("KatanaStageRecordManagerSubsystem: 기록 중인 스테이지와 종료 요청 스테이지가 다릅니다. Recording: %s, Request: %s"),
			*CurrentRecordingStageName.ToString(),
			*StageName.ToString()
		);
	}

	const float StageTime = GetCurrentStageTime();

	SaveStageTime(CurrentRecordingStageName, StageTime);

	bIsRecording = false;
	CurrentRecordingStageName = NAME_None;
	RecordStartTime = 0.0f;

	FinishStageTime = StageTime;

	UE_LOG(LogTemp, Log, TEXT("KatanaStageRecordManagerSubsystem: 스테이지 기록 종료 - %.2f초"), StageTime);

	return StageTime;
}

void UKatanaStageRecordManagerSubsystem::CancelStageRecord()
{
	bIsRecording = false;
	CurrentRecordingStageName = NAME_None;
	RecordStartTime = 0.0f;

	UE_LOG(LogTemp, Log, TEXT("KatanaStageRecordManagerSubsystem: 현재 스테이지 기록이 취소되었습니다."));
}

void UKatanaStageRecordManagerSubsystem::SaveStageTime(FName StageName, float StageTime)
{
	if (StageName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("KatanaStageRecordManagerSubsystem: StageName is None"));
		return;
	}

	if (StageTime <= 0.0f)
	{
		UE_LOG(LogTemp, Warning, TEXT("KatanaStageRecordManagerSubsystem: StageTime이 유효하지 않습니다. StageTime: %f"), StageTime);
		return;
	}

	const float* SavedBestTime = BestStageTimeMap.Find(StageName);
	if (SavedBestTime && *SavedBestTime <= StageTime)
	{
		UE_LOG(
			LogTemp,
			Log,
			TEXT("KatanaStageRecordManagerSubsystem: 기존 최고 기록이 더 빠릅니다. Stage: %s, Best: %.2f, Current: %.2f"),
			*StageName.ToString(),
			*SavedBestTime,
			StageTime
		);
		return;
	}

	BestStageTimeMap.Add(StageName, StageTime);
	SaveStageRecords();

	UE_LOG(
		LogTemp,
		Log,
		TEXT("KatanaStageRecordManagerSubsystem: 최고 스테이지 기록이 저장되었습니다. Stage: %s, StageTime: %.2f"),
		*StageName.ToString(),
		StageTime
	);
}

bool UKatanaStageRecordManagerSubsystem::TryGetBestStageTime(FName StageName, float& OutBestStageTime) const
{
	if (const float* SavedBestTime = BestStageTimeMap.Find(StageName))
	{
		OutBestStageTime = *SavedBestTime;
		return true;
	}

	OutBestStageTime = 0.0f;
	return false;
}

float UKatanaStageRecordManagerSubsystem::GetCurrentStageTime() const
{
	if (!bIsRecording)
		return 0.0f;

	const UWorld* World = GetWorld();
	if (!World)
		return 0.0f;

	return World->GetTimeSeconds() - RecordStartTime;
}

float UKatanaStageRecordManagerSubsystem::GetFinishStageTime() const
{
	return FinishStageTime;
}

bool UKatanaStageRecordManagerSubsystem::IsRecordingStage() const
{
	return bIsRecording;
}

void UKatanaStageRecordManagerSubsystem::LoadStageRecords()
{
	BestStageTimeMap.Reset();

	const UKatanaStageRecordSaveGame* SaveGameInstance = LoadStageRecordSaveGame(SlotName);

	if (!SaveGameInstance)
	{
		SaveGameInstance = CreateDefaultStageRecordSaveGame();
	}

	if (!SaveGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("KatanaStageRecordManagerSubsystem: StageRecordSaveGame 생성 실패"));
		return;
	}

	BestStageTimeMap = SaveGameInstance->BestStageTimeMap;

	UE_LOG(LogTemp, Log, TEXT("KatanaStageRecordManagerSubsystem: 스테이지 기록을 불러왔습니다."));
}

void UKatanaStageRecordManagerSubsystem::SaveStageRecords()
{
	TObjectPtr<UKatanaStageRecordSaveGame> SaveGameInstance = LoadStageRecordSaveGame(SlotName);

	if (!SaveGameInstance)
	{
		SaveGameInstance = CreateDefaultStageRecordSaveGame();
	}

	if (!SaveGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("KatanaStageRecordManagerSubsystem: StageRecordSaveGame 생성 실패"));
		return;
	}

	SaveGameInstance->BestStageTimeMap = BestStageTimeMap;

	if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotName, 0))
	{
		UE_LOG(LogTemp, Log, TEXT("KatanaStageRecordManagerSubsystem: 스테이지 기록이 저장되었습니다."));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("KatanaStageRecordManagerSubsystem: 스테이지 기록 저장 실패"));
	}
}

void UKatanaStageRecordManagerSubsystem::ResetStageRecord()
{
	UE_LOG(LogTemp, Log, TEXT("KatanaStageRecordManagerSubsystem: 스테이지 기록을 초기화합니다."));
	UGameplayStatics::DeleteGameInSlot(SlotName, 0);
	BestStageTimeMap.Reset();
}
