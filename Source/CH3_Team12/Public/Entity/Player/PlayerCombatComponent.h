#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/CombatTypes.h"
#include "Framework/DataAsset/PlayerCombatFeedbackDataAsset.h"
#include "PlayerCombatComponent.generated.h"

class UAnimMontage;
class APlayerCharacterBase;
class AWeaponBase;
class UStateTagComponent;
class UPlayerAttributeComponent;
class UNiagaraSystem;
class USoundBase;
class UPlayerCombatMontageDataAsset;

struct FInputActionValue;
struct FHitResult;

UENUM(BlueprintType)
enum class EDefenseResult : uint8
{
	None,
	Parry,
	Guard,
	Hit,
	Invincible
};

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
	void Attack(const FInputActionValue& Value);
	void HeavyAttack(const FInputActionValue& Value);

	// Attack Notify / NotifyState
	void OpenComboWindow();
	void EndAttack();

	void StartWeaponHitCheck();
	void WeaponTrace();
	void EndWeaponHitCheck();

	// Dodge Notify / NotifyState
	void EnableInvincible();
	void DisableInvincible();

	bool bInvincible = false;

	void EquipWeapon(TSubclassOf<AWeaponBase> WeaponClass);

	AWeaponBase* GetEquippedWeapon() const { return EquippedWeapon; }

	EDefenseResult ResolveIncomingAttack(const FIncomingAttackContext& Context);

	bool IsGuarding() const;
	bool IsParrying() const;

private:
	bool CanAttack() const;
	bool IsAttacking() const;
	bool IsBusy() const;

	bool CanContinueCombo() const;

	void StartAttack(EAttackType AttackType);
	void ContinueCombo();

	void ProcessHit(const FHitResult& Hit);
	void CacheWeaponTraceLocation();

private:
	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UStateTagComponent> StateComponent;
	
	UPROPERTY()
	TObjectPtr<UPlayerAttributeComponent> AttributeComponent;

private:
	UPROPERTY(EditDefaultsOnly, Category="Combat|Combo")
	TArray<FName> ComboSectionNames = {
		TEXT("Attack0"),
		TEXT("Attack1"),
	};

	UPROPERTY(EditDefaultsOnly, Category="Combat|Attack")
	float AttackPlayRate = 1.0f;

private:
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<AWeaponBase> DefaultWeaponClass;

	UPROPERTY(VisibleInstanceOnly, Category="Weapon")
	TObjectPtr<AWeaponBase> EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName WeaponSocketName = TEXT("katana3");

private:
	FVector PreviousBladeStart = FVector::ZeroVector;
	FVector PreviousBladeEnd = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Trace")
	float TraceRadius = 8.0f;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Trace")
	int32 TraceSampleCount = 5;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;

	TSet<TWeakObjectPtr<AActor>> HitActors;

private:
	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentAttackMontage;

	int32 ComboIndex = 0;

	bool bComboWindow = false;
	bool bComboBuffered = false;
	bool bWeaponHitCheck = false;

	EAttackType CurrentAttackType = EAttackType::Light;

private:
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

public:
	void StartGuard(const FInputActionValue& Value);
	void StopGuard(const FInputActionValue& Value);

	void OpenParryWindow();
	void CloseParryWindow();

private:
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

public:
	void OpenAttackRecovery();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsThrust = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsUnblockable = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCausesKnockback = false;
	
	UFUNCTION(BlueprintCallable, Category="Combat|Debug")
	void Debug_ReceiveTestAttackFront();

	UFUNCTION(BlueprintCallable, Category="Combat|Debug")
	void Debug_ReceiveTestAttackLeft();

	UFUNCTION(BlueprintCallable, Category="Combat|Debug")
	void Debug_ReceiveTestAttackRight();

	UFUNCTION(BlueprintCallable, Category="Combat|Debug")
	void Debug_ReceiveTestAttackBack();

private:
	void Debug_ReceiveTestAttack(EHitReactionDirection Direction);
	
	UPROPERTY(EditAnywhere, Category="Combat|Debug")
	float DebugAttackDamage = 10.0f;

	UPROPERTY(EditAnywhere, Category="Combat|Debug")
	float DebugAttackPostureDamage = 10.0f;
	
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
	
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> HitStopActors;
	FTimerHandle HitStopTimerHandle;

	void TriggerCombatHitStop(
		const FIncomingAttackContext& Context,
		float Duration,
		float TimeDilation
	);

	void ResetCombatHitStop();


	UPROPERTY(EditAnywhere, Category="Combat|Data")
	TObjectPtr<UPlayerCombatMontageDataAsset> MontageData;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Data")
	TObjectPtr<UPlayerCombatFeedbackDataAsset> FeedbackData;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Feedback")
	FName WeaponClashEffectSocketName = TEXT("katana_FXSocket");
	
	UPROPERTY(EditDefaultsOnly, Category="Combat|Feedback")
	float HitEffectSurfaceOffset = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Feedback")
	float FallbackEffectHeightOffset = 80.0f;
	
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
