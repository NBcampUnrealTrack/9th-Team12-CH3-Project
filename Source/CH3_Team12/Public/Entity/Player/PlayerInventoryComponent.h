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
	
	UPROPERTY(EditAnywhere)
	bool bAutoEquip = false;
	
	UPROPERTY(EditAnywhere)
	bool bRegisterConsumable = false;
};

DECLARE_MULTICAST_DELEGATE(FOnInventoryChanged);

DECLARE_MULTICAST_DELEGATE_OneParam(
	FOnItemAdded,
	UItemInstance*);

DECLARE_MULTICAST_DELEGATE_OneParam(
	FOnItemRemoved,
	UItemInstance*);

DECLARE_MULTICAST_DELEGATE_TwoParams(
	FOnConsumableSlotChanged,
	int32,
	UItemInstance*);

DECLARE_MULTICAST_DELEGATE_OneParam(
	FOnWeaponSlotChanged,
	UItemInstance*);

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
	
	int32 GetItemsCount() const { return Items.Num(); }
	
	int32 GetItemCount(const UItemDataAsset* ItemData) const;
	
	int32 GetConsumableSlotCount() const { return ConsumableSlots.Num(); }
	
	UItemInstance* GetConsumableSlot(int32 Index) const;
	
	UItemInstance* GetCurrentConsumable() const;
	
	UItemInstance* FindItem(
	const UItemDataAsset* ItemData) const;
	
	UItemInstance* GetEquippedWeaponInstance() const { return WeaponSlot; }
	
	bool SetWeaponSlot(UItemInstance* Item);
	
	void ClearWeaponSlot();
	
public:
	// delegate
	FOnInventoryChanged OnInventoryChanged;

	FOnItemAdded OnItemAdded;

	FOnItemRemoved OnItemRemoved;

	FOnConsumableSlotChanged OnConsumableSlotChanged;

	FOnWeaponSlotChanged OnWeaponSlotChanged;
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
