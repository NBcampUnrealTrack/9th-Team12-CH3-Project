// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/CombatTypes.h"
#include "Framework/DataAsset/PlayerDefenseDataAsset.h"
#include "Framework/DataAsset/PlayerCombatFeedbackDataAsset.h"
#include "PlayerDefenseComponent.generated.h"

class UAnimMontage;
class APlayerCharacterBase;
class AWeaponBase;
class UStateTagComponent;
class UPlayerAttributeComponent;
class UPlayerEquipmentComponent;
class UPlayerWeaponComponent;
class UNiagaraSystem;
class USoundBase;

struct FInputActionValue;
struct FHitResult;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH3_TEAM12_API UPlayerDefenseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerDefenseComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;
	UPROPERTY()
	TObjectPtr<UStateTagComponent> StateComponent;
	UPROPERTY()
	TObjectPtr<UPlayerAttributeComponent> AttributeComponent;
	UPROPERTY()
	TObjectPtr<UPlayerEquipmentComponent> EquipmentComponent;
	UPROPERTY()
	TObjectPtr<UPlayerWeaponComponent> WeaponComponent;
	
	UPROPERTY(EditDefaultsOnly, Category="Combat|Data")
	TObjectPtr<UPlayerDefenseDataAsset> DefenseData;
	UPROPERTY(EditDefaultsOnly, Category="Combat|Data")
	TObjectPtr<UPlayerCombatFeedbackDataAsset> FeedbackData;
	
public:
	void StartGuard(const FInputActionValue& Value);
	void StopGuard(const FInputActionValue& Value);
	
	bool CanGuard() const;
		
	void OpenParryWindow();
	void CloseParryWindow();

	EDefenseResult ResolveIncomingAttack(const FIncomingAttackContext& Context);
	
private:
	bool IsGuarding() const;
	bool IsParrying() const;
	
	void PlayParryReaction(EHitReactionDirection AttackDirection);
	void PlayGuardHitReaction(EHitReactionDirection AttackDirection);
	UAnimMontage* GetHitMontage(EHitReactionDirection ReactionDirection) const;
	void PlayHitReaction(EHitReactionDirection ReactionDirection);
	void OnHitReactionMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void EndHitReaction();
	
public:
	EHitReactionDirection CalculateHitReactionDirection(const FIncomingAttackContext& Context) const;

private:
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
	
	bool bInvincible = false;
	
public:
	void EnableInvincible();
	void DisableInvincible();
	
private:
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
	void PlayCombatFeedback(const FIncomingAttackContext& Context, const FCombatFeedbackData& Feedback);

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
	
};
