
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerTimeWarpComponent.generated.h"

class APlayerCharacterBase;
class UPlayerDefenseComponent;
class UPlayerTimeWarpDataAsset;
class UNiagaraComponent;

struct FIncomingAttackContext;

USTRUCT()
struct FTimeWarpDilationCache
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AActor> Actor = nullptr;

	float PreviousCustomTimeDilation = 1.0f;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CH3_TEAM12_API UPlayerTimeWarpComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerTimeWarpComponent();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

public:
	bool CanActivateTimeWarp() const;

	void ActivateTimeWarp(
		const FIncomingAttackContext& Context
	);

	void DeactivateTimeWarp();

	bool IsTimeWarpActive() const { return bActive; }

private:
	void HandleEvadeSuccess(
		const FIncomingAttackContext& Context
	);

	bool IsBlockedByState() const;

	void ApplyTimeScale();
	void RestoreTimeScale();

	void StartVisualEffects();
	void StopVisualEffects();

private:
	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UPlayerDefenseComponent> DefenseComponent;

	UPROPERTY(EditDefaultsOnly, Category="TimeWarp|Data")
	TObjectPtr<UPlayerTimeWarpDataAsset> TimeWarpData;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> ActiveTrailComponent;

	bool bActive = false;

	double EndRealTime = 0.0;
	double NextAvailableRealTime = 0.0;

	float CachedPreviousGlobalTimeDilation = 1.0f;
	float CachedPreviousPlayerCustomTimeDilation = 1.0f;
	
private:
	void ApplySelectiveTimeScale();
	void RestoreSelectiveTimeScale();

	bool ShouldAffectActor(AActor* Actor) const;
	bool IsActorAlreadyCached(AActor* Actor) const;

private:
	UPROPERTY()
	TArray<FTimeWarpDilationCache> AffectedActorCaches;
	
private:
	void UpdateVisualEffects();

	double VisualStartRealTime = 0.0;

	UPROPERTY(EditDefaultsOnly, Category="TimeWarp|Visual")
	float SurfaceSweepDuration = 0.65f;

	UPROPERTY(EditDefaultsOnly, Category="TimeWarp|Visual")
	float SweepStartDistance = -600.0f;

	UPROPERTY(EditDefaultsOnly, Category="TimeWarp|Visual")
	float SweepEndDistance = 3400.0f;

	UPROPERTY(EditDefaultsOnly, Category="TimeWarp|Visual")
	float SweepFadeOutStartAlpha = 0.85f;
};