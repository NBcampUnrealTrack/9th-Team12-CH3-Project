#pragma once

#include "CoreMinimal.h"
#include "Framework/DataAsset/ItemDataAsset.h"
#include "EquipmentDataAsset.generated.h"

UENUM(BlueprintType)
enum class EEquipmentSlot : uint8
{
	Weapon
	/*,
	Shield,
	Helmet,
	Armor,
	Gloves,
	Boots,
	Ring1,
	Ring2*/
};

UCLASS()
class CH3_TEAM12_API UEquipmentDataAsset : public UItemDataAsset
{
	GENERATED_BODY()
	
public:
	UEquipmentDataAsset()
	{
		bStackable = false;
		MaxStack = 1;
	}
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EEquipmentSlot EquipmentSlot = EEquipmentSlot::Weapon;
};
