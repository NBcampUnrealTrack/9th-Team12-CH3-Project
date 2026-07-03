#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCombatComponent.generated.h"

class UAnimMontage;
class APlayerCharacterBase;
class AWeaponBase;
struct FInputActionValue;
struct FHitResult;

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	Light,
	Heavy
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH3_TEAM12_API UPlayerCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerCombatComponent();

	virtual void BeginPlay() override;
	
	virtual void TickComponent(
		float DeltaTime, 
		enum ELevelTick TickType, 
		FActorComponentTickFunction* ThisTickFunction) override;

public:
	// Input
	void Attack(const FInputActionValue& value);
	void HeavyAttack(const FInputActionValue& value);
	void Dodge(const FInputActionValue& value);
	//
	
	// Notify
	void StartComboWindow();
	void EndComboWindow();
	
	void StartWeaponHitCheck();
	void EndWeaponHitCheck();
	
	void EnableInvincible();
	void DisableInvincible();
	
	void EndAttack();
	void EndDodge();
	//
	
public:
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TArray<FName> ComboSectionNames;
	
private:
	// Combat
	bool CanAttack() const;
	bool CanDodge() const;

	void StartAttack(EAttackType AttackType);
	void ContinueCombo();
	
	bool IsBusy() const;
	
private:
	// Trace
	void CacheWeaponTraceLocation();
	void ProcessHit(const FHitResult& Hit);
public:
	void WeaponTrace();
private:
	// Animation
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> LightAttackMontage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> HeavyAttackMontage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> DodgeMontage;
	
private:
	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;
	
	// Weapon
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	TSubclassOf<AWeaponBase> DefaultWeaponClass;
	
	UPROPERTY()
	TObjectPtr<AWeaponBase> EquippedWeapon;
	
	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	FName WeaponSocketName = TEXT("katana3");
	
private:
	void EquipWeapon(TSubclassOf<AWeaponBase> WeaponClass);
public:

	FORCEINLINE AWeaponBase* GetEquippedWeapon() const
	{
		return EquippedWeapon;
	}
private:
	// Trace
	FVector PreviousBladeStart;
	FVector PreviousBladeEnd;
	
	UPROPERTY(EditAnywhere, Category="Combat|Trace")
	float TraceRadius = 8.0f;
	UPROPERTY(EditAnywhere, Category="Combat|Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;
	
	TArray<FVector> PreviousSocketLocations;
	TSet<TWeakObjectPtr<AActor>> HitActors;
	
private:
	// Combo
	int32 ComboIndex = 0;

	bool bComboWindow = false;
	bool bComboBuffered = false;

private:
	// State
	bool bWeaponHitCheck = false;
	bool bInvincible = false;
};
