#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerTimeWarpDataAsset.generated.h"

class UNiagaraSystem;
class USoundBase;
class UMaterialInterface;

UCLASS(BlueprintType)
class CH3_TEAM12_API UPlayerTimeWarpDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TimeWarp")
	float AffectedActorTimeScale = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TimeWarp")
	TArray<TSubclassOf<AActor>> AffectedActorClasses;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TimeWarp")
	float Duration = 1.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TimeWarp")
	float GlobalTimeScale = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TimeWarp")
	float Cooldown = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TimeWarp")
	bool bRefreshDurationIfAlreadyActive = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TimeWarp|VFX")
	TObjectPtr<UNiagaraSystem> PlayerTrailEffect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TimeWarp|VFX")
	FName TrailAttachSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TimeWarp|Sound")
	TObjectPtr<USoundBase> StartSound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TimeWarp|Sound")
	TObjectPtr<USoundBase> EndSound = nullptr;

	// 1차 구현에서는 직접 쓰지 않고,
	// 나중에 CameraComponent/PostProcessComponent 쪽과 연결할 때 사용.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TimeWarp|PostProcess")
	TObjectPtr<UMaterialInterface> PostProcessMaterial = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="TimeWarp|PostProcess")
	float PostProcessBlendWeight = 1.0f;
};