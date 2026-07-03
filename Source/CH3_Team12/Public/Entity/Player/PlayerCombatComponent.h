#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCombatComponent.generated.h"

class UAnimMontage;
class APlayerCharacterBase;
struct FInputActionValue;

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
	
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	TArray<FName> ComboSectionNames;
private:
	bool CanAttack() const;
	bool CanDodge() const;

	void StartAttack(EAttackType AttackType);
	void ContinueCombo();
	
	bool IsBusy() const;
	
	void WeaponTrace();
private:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> LightAttackMontage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> HeavyAttackMontage;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UAnimMontage> DodgeMontage;
	
private:
	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;
	UPROPERTY(EditAnywhere, Category="Combat|Trace")
	TArray<FName> TraceSocketNames =
	{
		"BladeStart",
		"BladeEnd"
	};
	
	UPROPERTY(EditAnywhere, Category="Combat|Trace")
	float TraceRadius = 8.0f;
	
	UPROPERTY(EditAnywhere, Category="Combat|Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;
	
	TArray<FVector> PreviousSocketLocations;
	TSet<TWeakObjectPtr<AActor>> HitActors;
	
	int32 ComboIndex = 0;

	bool bComboWindow = false;
	bool bComboBuffered = false;

	bool bWeaponHitCheck = false;
	bool bInvincible = false;
};
