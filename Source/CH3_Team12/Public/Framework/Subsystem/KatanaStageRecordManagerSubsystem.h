#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "KatanaStageRecordManagerSubsystem.generated.h"

UCLASS()
class CH3_TEAM12_API UKatanaStageRecordManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	static UKatanaStageRecordManagerSubsystem* Get(const UObject* WorldContextObject);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable, Category = "Stage Record")
	void StartStageRecord(FName StageName);

	UFUNCTION(BlueprintCallable, Category = "Stage Record")
	float FinishStageRecord(FName StageName);

	UFUNCTION(BlueprintCallable, Category = "Stage Record")
	void CancelStageRecord();

	UFUNCTION(BlueprintCallable, Category = "Stage Record")
	void SaveStageTime(FName StageName, float StageTime);

	UFUNCTION(BlueprintCallable, Category = "Stage Record")
	bool TryGetBestStageTime(FName StageName, float& OutBestStageTime) const;

	UFUNCTION(BlueprintCallable, Category = "Stage Record")
	float GetCurrentStageTime() const;

	UFUNCTION(BlueprintCallable, Category = "Stage Record")
	float GetFinishStageTime() const;

	UFUNCTION(BlueprintCallable, Category = "Stage Record")
	bool IsRecordingStage() const;

	UFUNCTION(BlueprintCallable, Category = "Stage Record")
	void LoadStageRecords();

	UFUNCTION(BlueprintCallable, Category = "Stage Record")
	void SaveStageRecords();

	UFUNCTION(BlueprintCallable, Category = "Stage Record")
	void ResetStageRecord();

private:
	UPROPERTY()
	TMap<FName, float> BestStageTimeMap;

	UPROPERTY()
	FName CurrentRecordingStageName = NAME_None;

	UPROPERTY()
	float RecordStartTime = 0.0f;

	UPROPERTY()
	bool bIsRecording = false;

	UPROPERTY()
	float FinishStageTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Stage Record")
	FString SlotName = TEXT("KatanaStageRecordSettings");
};