#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/CombatTypes.h"
#include "PlayerCombatComponent.generated.h"

class UAnimMontage;
class APlayerCharacterBase;
class AWeaponBase;
class UStateTagComponent;
class UPlayerAttributeComponent;
class UNiagaraSystem;
class USoundBase;

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

UENUM(BlueprintType)
enum class ECombatEffectRotationMode : uint8
{
	None,
	ImpactNormal,
	AttackDirection,
	OppositeAttackDirection,
	AttackerToDefender,
	DefenderToAttacker,
	DefenderForward
};

UENUM(BlueprintType)
enum class ECombatEffectLocationMode : uint8
{
	HitImpactPoint,
	DefenderWeaponClashSocket,
	DefenderWeaponBladeMiddle,
	DefenderActorCenter
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

	UFUNCTION(BlueprintCallable, Category="Weapon")
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
	UPROPERTY(EditDefaultsOnly, Category="Combat|Animation")
	TObjectPtr<UAnimMontage> LightAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Animation")
	TObjectPtr<UAnimMontage> HeavyAttackMontage;

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
	TObjectPtr<UAnimMontage> GuardStartMontage;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Guard")
	float GuardMontageBlendOutTime = 0.1f;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Parry")
	TObjectPtr<UAnimMontage> ParryLeftMontage;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Parry")
	TObjectPtr<UAnimMontage> ParryRightMontage;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Guard")
	TObjectPtr<UAnimMontage> GuardHitLeftMontage;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Guard")
	TObjectPtr<UAnimMontage> GuardHitRightMontage;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Hit")
	TObjectPtr<UAnimMontage> HitFrontMontage;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Hit")
	TObjectPtr<UAnimMontage> HitLeftMontage;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Hit")
	TObjectPtr<UAnimMontage> HitRightMontage;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Hit")
	TObjectPtr<UAnimMontage> HitBackMontage;
	
	void PlayParryReaction(EHitReactionDirection AttackDirection);
	void PlayGuardHitReaction(EHitReactionDirection AttackDirection);
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

	void SpawnParryEffect(const FIncomingAttackContext& Context);
	void SpawnGuardHitEffect(const FIncomingAttackContext& Context);
	void SpawnHitEffect(const FIncomingAttackContext& Context);
	
private:
	UPROPERTY(EditAnywhere, Category="Combat|Feedback")
	float ParryHitStopDuration = 0.05f;

	UPROPERTY(EditAnywhere, Category="Combat|Feedback")
	float ParryHitStopTimeDilation = 0.05f;

	UPROPERTY(EditAnywhere, Category="Combat|Feedback")
	float GuardHitStopDuration = 0.035f;

	UPROPERTY(EditAnywhere, Category="Combat|Feedback")
	float GuardHitStopTimeDilation = 0.1f;

	UPROPERTY(EditAnywhere, Category="Combat|Feedback")
	float HitStopDuration = 0.04f;

	UPROPERTY(EditAnywhere, Category="Combat|Feedback")
	float HitStopTimeDilation = 0.08f;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> HitStopActors;

	FTimerHandle HitStopTimerHandle;

private:
	void TriggerCombatHitStop(
		const FIncomingAttackContext& Context,
		float Duration,
		float TimeDilation
	);

	void ResetCombatHitStop();

// Test용도 : Effect, Sound
private:
	UPROPERTY(EditDefaultsOnly, Category="Combat|Feedback|VFX")
	TObjectPtr<UNiagaraSystem> ParryEffect;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Feedback|VFX")
	TObjectPtr<UNiagaraSystem> GuardHitEffect;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Feedback|VFX")
	TObjectPtr<UNiagaraSystem> HitEffect;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Feedback|SFX")
	TObjectPtr<USoundBase> ParrySound;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Feedback|SFX")
	TObjectPtr<USoundBase> GuardHitSound;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Feedback|SFX")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Feedback")
	float FeedbackEffectForwardOffset = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Feedback")
	float FeedbackEffectHeightOffset = 60.0f;

	FVector GetFeedbackLocation(const FIncomingAttackContext& Context) const;
	FRotator GetFeedbackRotation(const FIncomingAttackContext& Context) const;
	
	FVector GetWeaponClashEffectLocation(const FIncomingAttackContext& Context) const;
	FVector GetHitEffectLocation(const FIncomingAttackContext& Context) const;
	FRotator MakeCombatEffectRotation(const FIncomingAttackContext& Context,
	                                  ECombatEffectRotationMode RotationMode) const;

	FName GuardSocketName = TEXT("katana_FXSocket");
};
