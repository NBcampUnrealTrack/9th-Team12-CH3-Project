#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/CombatTypes.h"
#include "Framework/DataAsset/PlayerCombatFeedbackDataAsset.h"
#include "CombatFeedbackComponent.generated.h"

class APlayerCharacterBase;
class UPlayerEquipmentComponent;

USTRUCT(BlueprintType)
struct FCombatHitStopSpec
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AActor> Attacker = nullptr;

	UPROPERTY()
	TObjectPtr<AActor> Defender = nullptr;

	UPROPERTY()
	float Duration = 0.0f;

	UPROPERTY()
	float AttackerTimeDilation = 1.0f;

	UPROPERTY()
	float DefenderTimeDilation = 1.0f;

	UPROPERTY()
	bool bAffectAttacker = false;

	UPROPERTY()
	bool bAffectDefender = false;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CH3_TEAM12_API UCombatFeedbackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatFeedbackComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	void PlayCombatFeedback(
		const FIncomingAttackContext& Context,
		const UPlayerCombatFeedbackDataAsset* FeedbackAsset,
		const FCombatFeedbackData& Feedback
	);

	void ApplyHitStop(const FCombatHitStopSpec& Spec);
	void ResetHitStop();

private:
	void ApplyHitStopFromFeedback(
		const FIncomingAttackContext& Context,
		const FCombatFeedbackData& Feedback
	);

	void ApplyTimeDilationToActor(
		AActor* Actor,
		float TimeDilation
	);

	FVector MakeCombatEffectLocation(
		const FIncomingAttackContext& Context,
		const UPlayerCombatFeedbackDataAsset* FeedbackAsset,
		ECombatEffectLocationMode LocationMode
	) const;

	FVector GetWeaponClashEffectLocation(
		const FIncomingAttackContext& Context,
		const UPlayerCombatFeedbackDataAsset* FeedbackAsset
	) const;

	FVector GetHitImpactEffectLocation(
		const FIncomingAttackContext& Context,
		const UPlayerCombatFeedbackDataAsset* FeedbackAsset
	) const;

	FVector GetFallbackEffectLocation(
		const UPlayerCombatFeedbackDataAsset* FeedbackAsset
	) const;

	FRotator MakeCombatEffectRotation(
		const FIncomingAttackContext& Context,
		ECombatEffectRotationMode RotationMode
	) const;

private:
	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UPlayerEquipmentComponent> EquipmentComponent;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> HitStopActors;

	FTimerHandle HitStopTimerHandle;
};