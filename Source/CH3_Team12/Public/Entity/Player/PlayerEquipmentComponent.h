#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerEquipmentComponent.generated.h"

class UStateTagComponent;
class UPlayerInventoryComponent;
struct FInputActionValue;
class UAnimMontage;
class UWeaponDataAsset;
class APlayerCharacterBase;
class UItemInstance;
class AWeaponBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH3_TEAM12_API UPlayerEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerEquipmentComponent();

protected:
	virtual void BeginPlay() override;
	
public:
	void ToggleWeaponInput(const FInputActionValue& Value);
	
	/** AnimNotify에서 호출 */
	void AnimNotify_Equip();

	/** AnimNotify에서 호출 */
	void AnimNotify_Unequip();
	
	/** 현재 장착된 무기 Actor */
	FORCEINLINE AWeaponBase* GetEquippedWeapon() const
	{
		return EquippedWeapon;
	}
	
	/** 현재 장착된 아이템 */
	FORCEINLINE UItemInstance* GetEquippedWeaponItem() const
	{
		return EquippedWeaponItem;
	}

private:
	bool CanChangeWeapon() const;
	
	bool Equip(UItemInstance* Item);

	void Unequip();
	
	bool EquipWeapon(UItemInstance* Item, const UWeaponDataAsset* WeaponData);
	
	void UnequipWeapon();
	
	void OnEquipMontageEnded(
		UAnimMontage* Montage,
		bool bInterrupted);

	void OnUnequipMontageEnded(
		UAnimMontage* Montage,
		bool bInterrupted);
private:
	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UPlayerInventoryComponent> Inventory;
	
	UPROPERTY()
	TObjectPtr<UStateTagComponent> StateComp;
	
	/** 현재 장착중인 ItemInstance */
	UPROPERTY()
	TObjectPtr<UItemInstance> EquippedWeaponItem;

	/** 현재 장착중인 무기 Actor */
	UPROPERTY()
	TObjectPtr<AWeaponBase> EquippedWeapon;
		
	/** 장착 예정 Item */
	UPROPERTY()
	TObjectPtr<UItemInstance> PendingEquipItem;
};
