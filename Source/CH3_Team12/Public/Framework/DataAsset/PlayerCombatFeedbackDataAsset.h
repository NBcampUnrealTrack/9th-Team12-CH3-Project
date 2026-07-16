#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerCombatFeedbackDataAsset.generated.h"

class UNiagaraSystem;
class USoundBase;

UENUM(BlueprintType)
enum class ECombatHitStopTargetPolicy : uint8
{
	None,
	DefenderOnly,
	AttackerOnly,
	Both
};

UENUM(BlueprintType)
enum class ECombatEffectLocationMode : uint8
{
	HitImpactPoint,
	DefenderWeaponClashSocket,
	DefenderWeaponBladeMiddle,
	DefenderActorCenter
};

UENUM(BlueprintType)
enum class ECombatEffectRotationMode : uint8
{
	None,
	ImpactNormal,
	AttackDirection,
	OppositeAttackDirection,
	AttackerToDefender,
	DefenderToAttacker,
	DefenderForward,
	WorldUp
};

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
	float HitStopDuration = 0.15f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ECombatHitStopTargetPolicy HitStopTargetPolicy =
		ECombatHitStopTargetPolicy::Both;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AttackerTimeDilation = 0.08f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DefenderTimeDilation = 0.08f;
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
	
	UPROPERTY(EditDefaultsOnly, Category="Combat|Feedback")
	FName WeaponClashEffectSocketName = TEXT("weapon_r_FXSocket");
	
	UPROPERTY(EditDefaultsOnly, Category="Combat|Feedback")
	float HitEffectSurfaceOffset = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Feedback")
	float FallbackEffectHeightOffset = 20.0f;
};