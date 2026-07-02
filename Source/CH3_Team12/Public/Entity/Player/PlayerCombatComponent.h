#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCombatComponent.generated.h"

class UAnimMontage;

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
	void Attack();
	void HeavyAttack();
	void Dodge();
	//
	
	// Notify
	void StartComboWindow();
	void EndComboWindow();

	void EnableWeaponCollision();
	void DisableWeaponCollision();
	
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
	
	
	int32 ComboIndex = 0;

	UPROPERTY(EditAnywhere)
	int32 MaxComboCount = 4;
	
	bool bComboWindow = false;
	bool bComboBuffered = false;

	bool bWeaponCollision = false;
	bool bInvincible = false;
};
