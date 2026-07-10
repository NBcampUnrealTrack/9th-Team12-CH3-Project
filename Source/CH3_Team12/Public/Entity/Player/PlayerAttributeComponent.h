#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Framework/DataAsset/PlayerAttributeDataAsset.h"
#include "PlayerAttributeComponent.generated.h"

class APlayerCharacterBase;
class UStateTagComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnPlayerAttributeChanged,
	float,
	CurrentValue,
	float,
	MaxValue
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FOnPlayerPostureBroken
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FOnPlayerPostureRecovered
);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FOnPlayerDeath
);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CH3_TEAM12_API UPlayerAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerAttributeComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

public:
	UPROPERTY(BlueprintAssignable, Category="Player|Attribute")
	FOnPlayerAttributeChanged OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="Player|Attribute")
	FOnPlayerAttributeChanged OnPostureChanged;

	UPROPERTY(BlueprintAssignable, Category="Player|Attribute")
	FOnPlayerPostureBroken OnPostureBroken;

	UPROPERTY(BlueprintAssignable, Category="Player|Attribute")
	FOnPlayerPostureRecovered OnPostureRecovered;

	UPROPERTY(BlueprintAssignable, Category="Player|Attribute")
	FOnPlayerDeath OnDeath;

public:
	UFUNCTION(BlueprintCallable, Category="Player|Attribute")
	void ApplyAttributeDamage(
		float HealthDamage,
		float PostureDamage
	);

	UFUNCTION(BlueprintCallable, Category="Player|Attribute")
	void ApplyHealthDamage(float HealthDamage);

	UFUNCTION(BlueprintCallable, Category="Player|Attribute")
	void ApplyPostureDamage(float PostureDamage);

	UFUNCTION(BlueprintCallable, Category="Player|Attribute")
	void Heal(float HealAmount);

	UFUNCTION(BlueprintCallable, Category="Player|Attribute")
	void RecoverPosture(float RecoveryAmount);

	UFUNCTION(BlueprintCallable, Category="Player|Attribute")
	void ResetAttributes();

public:
	UFUNCTION(BlueprintPure, Category="Player|Attribute")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category="Player|Attribute")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category="Player|Attribute")
	float GetCurrentPosture() const { return CurrentPosture; }

	UFUNCTION(BlueprintPure, Category="Player|Attribute")
	float GetMaxPosture() const { return MaxPosture; }

	UFUNCTION(BlueprintPure, Category="Player|Attribute")
	bool IsDead() const { return bIsDead; }

	UFUNCTION(BlueprintPure, Category="Player|Attribute")
	bool IsPostureBroken() const { return bIsPostureBroken; }

private:
	void UpdatePostureRecovery(float DeltaTime);
	void BreakPosture();
	void RecoverFromPostureBreak();
	void Die();

private:
	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UStateTagComponent> StateComponent;
	
	UPROPERTY(EditDefaultsOnly, Category="Player|Attribute|Data")
	TObjectPtr<UPlayerAttributeDataAsset> AttributeData;

private:
	float MaxHealth = 0.0f;
	float MaxPosture = 0.0f;
	float PostureRecoveryRate = 0.0f;
	float PostureRecoveryDelay = 0.0f;
	float PostureBreakDuration = 0.0f;

	float CurrentHealth = 0.0f;
	float CurrentPosture = 0.0f;
private:
	float LastPostureDamageTime = -999.0f;

	bool bIsDead = false;
	bool bIsPostureBroken = false;

	FTimerHandle PostureBreakTimerHandle;
};