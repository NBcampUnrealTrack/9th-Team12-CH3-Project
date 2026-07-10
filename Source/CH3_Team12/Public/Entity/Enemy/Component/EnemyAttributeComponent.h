// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Framework/DataAsset/EnemyAttributeDataAsset.h"
#include "EnemyAttributeComponent.generated.h"

class AEnemyCharacterBase;
class UStateTagComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnEnemyAttributeChanged,
	float,
	CurrentValue,
	float,
	MaxValue
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FOnEnemyPostureBroken
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FOnEnemyPostureRecovered
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FOnEnemyDeath
);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH3_TEAM12_API UEnemyAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyAttributeComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

public:
	UPROPERTY(BlueprintAssignable, Category="Enemy|Attribute")
	FOnEnemyAttributeChanged OnEnemyHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="Enemy|Attribute")
	FOnEnemyAttributeChanged OnEnemyPostureChanged;

	UPROPERTY(BlueprintAssignable, Category="Enemy|Attribute")
	FOnEnemyPostureBroken OnEnemyPostureBroken;

	UPROPERTY(BlueprintAssignable, Category="Enemy|Attribute")
	FOnEnemyPostureRecovered OnEnemyPostureRecovered;

	UPROPERTY(BlueprintAssignable, Category="Enemy|Attribute")
	FOnEnemyDeath OnEnemyDeath;

public:
	UFUNCTION(BlueprintCallable, Category="Enemy|Attribute")
	void ApplyAttributeDamage(
		float HealthDamage,
		float PostureDamage
	);

	UFUNCTION(BlueprintCallable, Category="Enemy|Attribute")
	void ApplyHealthDamage(float HealthDamage);

	UFUNCTION(BlueprintCallable, Category="Enemy|Attribute")
	void ApplyPostureDamage(float PostureDamage);

	UFUNCTION(BlueprintCallable, Category="Enemy|Attribute")
	void Heal(float HealAmount);

	UFUNCTION(BlueprintCallable, Category="Enemy|Attribute")
	void RecoverPosture(float RecoveryAmount);

	UFUNCTION(BlueprintCallable, Category="Enemy|Attribute")
	void ResetAttributes();

public:
	UFUNCTION(BlueprintPure, Category="Enemy|Attribute")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category="Enemy|Attribute")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category="Enemy|Attribute")
	float GetCurrentPosture() const { return CurrentPosture; }

	UFUNCTION(BlueprintPure, Category="Enemy|Attribute")
	float GetMaxPosture() const { return MaxPosture; }

	UFUNCTION(BlueprintPure, Category="Enemy|Attribute")
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintPure, Category="Enemy|Attribute")
	bool IsPostureBroken() const { return bIsPostureBroken; }

private:
	void UpdatePostureRecovery(float DeltaTime);
	void BreakPosture();
	void RecoverFromPostureBreak();
	void Die();

private:
	UPROPERTY()
	TObjectPtr<AEnemyCharacterBase> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UStateTagComponent> StateComponent;
	
	UPROPERTY(EditDefaultsOnly, Category="Enemy|Attribute|Data")
	TObjectPtr<UEnemyAttributeDataAsset> AttributeData;

private:
	float MaxHealth = 100.0f;
	float MaxPosture = 100.0f;
	float PostureRecoveryRate = 20.0f;
	float PostureRecoveryDelay = 2.0f;
	float PostureBreakDuration = 1.5f;

	float CurrentHealth = 0.0f;
	float CurrentPosture = 0.0f;
private:
	float LastPostureDamageTime = -999.0f;

	bool bIsDead = false;
	bool bIsPostureBroken = false;

	FTimerHandle PostureBreakTimerHandle;
};
