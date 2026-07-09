#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Combat/CombatTypes.h"
#include "PlayerCombatFeedbackDataAsset.generated.h"

class UNiagaraSystem;
class USoundBase;

USTRUCT(BlueprintType)
struct FCombatFeedbackData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UNiagaraSystem> Effect = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USoundBase> Sound = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ECombatEffectLocationMode LocationMode =
		ECombatEffectLocationMode::HitImpactPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ECombatEffectRotationMode RotationMode =
		ECombatEffectRotationMode::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HitStopDuration = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HitStopTimeDilation = 1.0f;
};

UCLASS(BlueprintType)
class CH3_TEAM12_API UPlayerCombatFeedbackDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Feedback")
	FCombatFeedbackData ParryFeedback;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Feedback")
	FCombatFeedbackData GuardFeedback;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Feedback")
	FCombatFeedbackData HitFeedback;
};