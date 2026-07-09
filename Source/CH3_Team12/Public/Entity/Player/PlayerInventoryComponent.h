#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerInventoryComponent.generated.h"

class UConsumableDataAsset;
struct FInputActionValue;
class UItemInstance;
class UItemDataAsset;

USTRUCT(BlueprintType)
struct FStarterItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UItemDataAsset* Item = nullptr;

	UPROPERTY(EditAnywhere)
	int32 Count = 1;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UPlayerInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerInventoryComponent();
	
	UItemInstance* AddItem(UItemDataAsset* ItemData, int32 Count = 1);

	bool RemoveItem(UItemInstance* Item, int32 Count);

	bool AddConsumableSlot(UItemInstance* Item);

	bool RemoveConsumableSlot(UItemInstance* Item);

	void SetConsumableSlot(int32 SlotIndex, UItemInstance* Item);
	
	/** 모든 아이템 */
	const TArray<TObjectPtr<UItemInstance>>& GetItems() const { return Items; }

	UItemInstance* GetCurrentConsumable() const;
	
	UItemInstance* GetCurrentWeapon() const;
	
	bool SetWeaponSlot(UItemInstance* Item);
	
	void ClearWeaponSlot();
	
protected:
	virtual void BeginPlay() override;

private:
	void RemoveItemFromConsumableSlots(UItemInstance* Item);
	
	// 인벤토리
	UPROPERTY()
	TArray<TObjectPtr<UItemInstance>> Items;
	
	UPROPERTY()
	TArray<TObjectPtr<UItemInstance>> ConsumableSlots;

	UPROPERTY()
	TObjectPtr<UItemInstance> WeaponSlot = nullptr;
	
	UPROPERTY()
	int32 CurrentConsumableIndex = 0;
	
private:
	UPROPERTY(EditDefaultsOnly)
	TArray<FStarterItem> StarterItems;
};
