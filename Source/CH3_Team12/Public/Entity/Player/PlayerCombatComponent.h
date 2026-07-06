#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCombatComponent.generated.h"

class UAnimMontage;
class APlayerCharacterBase;
class AWeaponBase;
class UStateTagComponent;
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

private:
	UPROPERTY(EditDefaultsOnly, Category="Combat|Guard")
	TObjectPtr<UAnimMontage> GuardStartMontage;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Guard")
	float GuardMontageBlendOutTime = 0.1f;
	
public:
	void OpenAttackRecovery();

};