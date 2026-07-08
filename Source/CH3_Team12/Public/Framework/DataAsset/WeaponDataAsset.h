#pragma once

#include "CoreMinimal.h"
#include "Framework/DataAsset/EquipmentDataAsset.h"
#include "WeaponDataAsset.generated.h"

class UAnimMontage;
class AWeaponBase;

UCLASS()
class CH3_TEAM12_API UWeaponDataAsset : public UEquipmentDataAsset
{
	GENERATED_BODY()
	
public:
	/** 실제 Spawn할 무기 Actor */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	TSubclassOf<AWeaponBase> WeaponClass;

	/** 무기 공격력 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon")
	float AttackPower = 10.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation")
	TObjectPtr<UAnimMontage> EquipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animation")
	TObjectPtr<UAnimMontage> UnequipMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName EquipSocket = TEXT("katana3");
};
