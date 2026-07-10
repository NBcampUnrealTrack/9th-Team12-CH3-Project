#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/CombatTypes.h"
#include "Framework/DataAsset/PlayerCombatMontageDataAsset.h"
#include "Framework/DataAsset/PlayerCombatFeedbackDataAsset.h"
#include "PlayerCombatComponent.generated.h"

class UAnimMontage;
class APlayerCharacterBase;
class AWeaponBase;
class UStateTagComponent;
class UPlayerAttributeComponent;
class UPlayerEquipmentComponent;
class UNiagaraSystem;
class USoundBase;

struct FInputActionValue;
struct FHitResult;

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	Light,
	Heavy
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CH3_TEAM12_API UPlayerCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerCombatComponent();

protected:
	virtual void BeginPlay() override;

public:
	// Input
	void Attack(const FInputActionValue& Value);
	void HeavyAttack(const FInputActionValue& Value);
	
	void StartGuard(const FInputActionValue& Value);
	void StopGuard(const FInputActionValue& Value);

	// Notify / NotifyState
	void OpenComboWindow();
	void EndAttack();
	void OpenAttackRecovery();
	void StartWeaponHitCheck();
	void WeaponTrace();
	void EndWeaponHitCheck();

	bool bInvincible = false;
	void EnableInvincible();
	void DisableInvincible();

	void OpenParryWindow();
	void CloseParryWindow();

	EDefenseResult ResolveIncomingAttack(const FIncomingAttackContext& Context);

	UPROPERTY(EditAnywhere, Category="Combat|Data")
	TObjectPtr<UPlayerCombatMontageDataAsset> MontageData;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Data")
	TObjectPtr<UPlayerCombatFeedbackDataAsset> FeedbackData;

private:
	bool IsGuarding() const;
	bool IsParrying() const;
	
	bool CanAttack() const;
	bool IsAttacking() const;
	bool IsBusy() const;

	bool CanContinueCombo() const;

	void StartAttack(EAttackType AttackType);
	void ContinueCombo();

	void ProcessHit(const FHitResult& Hit);
	void CacheWeaponTraceLocation();

	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UStateTagComponent> StateComponent;
	
	UPROPERTY()
	TObjectPtr<UPlayerAttributeComponent> AttributeComponent;
	
	UPROPERTY()
	TObjectPtr<UPlayerEquipmentComponent> EquipmentComponent;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Combo")
	TArray<FName> ComboSectionNames = {
		TEXT("Attack0"),
		TEXT("Attack1"),
		TEXT("Attack2"),
	};

	UPROPERTY(EditDefaultsOnly, Category="Combat|Attack")
	float AttackPlayRate = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName WeaponSocketName = TEXT("weapon_rSocket");

	FVector PreviousBladeStart = FVector::ZeroVector;
	FVector PreviousBladeEnd = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Trace")
	float TraceRadius = 8.0f;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Trace")
	int32 TraceSampleCount = 5;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;

	TSet<TWeakObjectPtr<AActor>> HitActors;
	
	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentAttackMontage;

	int32 ComboIndex = 0;

	bool bComboWindow = false;
	bool bComboBuffered = false;
	bool bWeaponHitCheck = false;

	EAttackType CurrentAttackType = EAttackType::Light;
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:
	// Debug
	UFUNCTION(BlueprintCallable, Category="Combat|Debug")
	void Debug_ReceiveTestAttackFront();

	UFUNCTION(BlueprintCallable, Category="Combat|Debug")
	void Debug_ReceiveTestAttackLeft();

	UFUNCTION(BlueprintCallable, Category="Combat|Debug")
	void Debug_ReceiveTestAttackRight();

	UFUNCTION(BlueprintCallable, Category="Combat|Debug")
	void Debug_ReceiveTestAttackBack();

	void Debug_ReceiveTestAttack(EHitReactionDirection Direction);
	
	UPROPERTY(EditAnywhere, Category="Combat|Debug")
	float DebugAttackDamage = 10.0f;

	UPROPERTY(EditAnywhere, Category="Combat|Debug")
	float DebugAttackPostureDamage = 10.0f;
	
private:
	// Guard
	bool CanGuard() const;
	
	UPROPERTY(EditDefaultsOnly, Category="Combat|Guard")
	float GuardMontageBlendOutTime = 0.1f;
	
	void PlayParryReaction(EHitReactionDirection AttackDirection);
	void PlayGuardHitReaction(EHitReactionDirection AttackDirection);
	UAnimMontage* GetHitMontage(EHitReactionDirection ReactionDirection) const;
	void PlayHitReaction(EHitReactionDirection ReactionDirection);
	void OnHitReactionMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void EndHitReaction();

	EHitReactionDirection CalculateHitReactionDirection(const FIncomingAttackContext& Context) const;

	UPROPERTY(EditAnywhere, Category="Combat|Guard")
	float GuardChipDamageRate = 0.2f;
	
	UPROPERTY(EditAnywhere, Category="Combat|Guard")
	float GuardPostureDamageRate = 1.0f;

	void PlayCombatFeedback(const FIncomingAttackContext& Context, const FCombatFeedbackData& Feedback);

	void HandleParrySuccess(
		const FIncomingAttackContext& Context,
		EHitReactionDirection ReactionDirection
	);
	void HandleGuardSuccess(
		const FIncomingAttackContext& Context,
		EHitReactionDirection ReactionDirection
	);
	void HandleDirectHit(
		const FIncomingAttackContext& Context,
		EHitReactionDirection ReactionDirection
	);
	
	// HitStop
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> HitStopActors;
	FTimerHandle HitStopTimerHandle;

	void TriggerCombatHitStop(
		const FIncomingAttackContext& Context,
		float Duration,
		float TimeDilation
	);

	void ResetCombatHitStop();

	// Effect
	FVector MakeCombatEffectLocation(
		const FIncomingAttackContext& Context,
		ECombatEffectLocationMode LocationMode
	) const;

	FVector GetWeaponClashEffectLocation(
		const FIncomingAttackContext& Context
	) const;

	FVector GetHitImpactEffectLocation(
		const FIncomingAttackContext& Context
	) const;

	FVector GetFallbackEffectLocation() const;
	
	FRotator MakeCombatEffectRotation(const FIncomingAttackContext& Context,
									  ECombatEffectRotationMode RotationMode) const;
};
