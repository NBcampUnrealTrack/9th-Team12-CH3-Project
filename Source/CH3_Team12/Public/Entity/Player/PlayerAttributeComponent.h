#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
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

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player|Attribute|Health", meta=(AllowPrivateAccess="true"))
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Player|Attribute|Health", meta=(AllowPrivateAccess="true"))
	float CurrentHealth = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player|Attribute|Posture", meta=(AllowPrivateAccess="true"))
	float MaxPosture = 100.0f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Player|Attribute|Posture", meta=(AllowPrivateAccess="true"))
	float CurrentPosture = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player|Attribute|Posture", meta=(AllowPrivateAccess="true"))
	float PostureRecoveryRate = 20.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player|Attribute|Posture", meta=(AllowPrivateAccess="true"))
	float PostureRecoveryDelay = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Player|Attribute|Posture", meta=(AllowPrivateAccess="true"))
	float PostureBreakDuration = 1.5f;

private:
	float LastPostureDamageTime = -999.0f;

	bool bIsDead = false;
	bool bIsPostureBroken = false;

	FTimerHandle PostureBreakTimerHandle;
};