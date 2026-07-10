#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/CombatTypes.h"
#include "Framework/DataAsset/PlayerAttackDataAsset.h"
#include "PlayerAttackComponent.generated.h"

class APlayerCharacterBase;
class UStateTagComponent;
class UPlayerAttributeComponent;
class UPlayerEquipmentComponent;
class UPlayerWeaponComponent;

struct FInputActionValue;
struct FHitResult;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CH3_TEAM12_API UPlayerAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerAttackComponent();

protected:
	virtual void BeginPlay() override;

public:
	// Input
	void Attack(const FInputActionValue& Value);
	void HeavyAttack(const FInputActionValue& Value);

	// Notify / NotifyState
	void OpenComboWindow();
	void EndAttack();
	void OpenAttackRecovery();

	UPROPERTY(EditAnywhere, Category="Combat|Data")
	TObjectPtr<UPlayerAttackDataAsset> AttackData;

private:
	bool CanAttack() const;
	bool IsAttacking() const;
	bool IsBusy() const;

	bool CanContinueCombo() const;

	void StartAttack(EAttackType AttackType);
	void ContinueCombo();

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
	
	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentAttackMontage;

	int32 ComboIndex = 0;

	bool bComboWindow = false;
	bool bComboBuffered = false;

	EAttackType CurrentAttackType = EAttackType::Light;
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
};
