#include "Entity/Player/PlayerInventoryComponent.h"
#include "Framework/DataAsset/ItemDataAsset.h"
#include "Framework/DataAsset/ConsumableDataAsset.h"
#include "Framework/DataAsset/WeaponDataAsset.h"
#include "Entity/Item/ItemEffect.h"
#include "Entity/Item/ItemInstance.h"

UPlayerInventoryComponent::UPlayerInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// 시작 아이템 지급
	for (const FStarterItem& StarterItem : StarterItems)
	{
		UItemInstance* Item = AddItem(
			StarterItem.Item,
			StarterItem.Count);

		if (!Item)
		{
			continue;
		}

		if (Cast<UWeaponDataAsset>(StarterItem.Item))
		{
			SetWeaponSlot(Item);
		}
		else if (Cast<UConsumableDataAsset>(StarterItem.Item))
		{
			SetConsumableSlot(0, Item); // 추후 AddConsumableSlot으로 교체
		}
	}
	//
	
}

UItemInstance* UPlayerInventoryComponent::AddItem(UItemDataAsset* ItemData, int32 Count)
{
	if (!ItemData || Count <= 0)
	{
		return nullptr;
	}

	if (!ItemData->bStackable)
	{
		UItemInstance* LastCreatedItem = nullptr;
		
		for (int32 i = 0; i < Count; ++i)
		{
			UItemInstance* NewItem = NewObject<UItemInstance>(this);
			NewItem->Initialize(ItemData, 1);

			Items.Add(NewItem);
			LastCreatedItem = NewItem;
		}

		return LastCreatedItem;
	}
	
	int32 Remaining = Count;
	
	for (UItemInstance* Item : Items)
	{
		if (!Item)
		{
			continue;
		}

		if (Item->GetItemData() != ItemData)
		{
			continue;
		}

		const int32 Space = ItemData->MaxStack - Item->GetCount();

		if (Space <= 0)
		{
			continue;
		}

		const int32 AddAmount = FMath::Min(Space, Remaining);

		Item->AddCount(AddAmount);

		Remaining -= AddAmount;

		if (Remaining <= 0)
		{
			return Item;
		}
	}

	UItemInstance* FirstNewItem = nullptr;
	
	while (Remaining > 0)
	{
		const int32 StackCount = FMath::Min(Remaining, ItemData->MaxStack);

		UItemInstance* NewItem = NewObject<UItemInstance>(this);
		NewItem->Initialize(ItemData, StackCount);

		Items.Add(NewItem);

		if (!FirstNewItem)
		{
			FirstNewItem = NewItem;
		}
		
		Remaining -= StackCount;
	}

	return FirstNewItem;
}

bool UPlayerInventoryComponent::RemoveItem(UItemInstance* Item, int32 Count)
{
	if (!Item || Count <= 0 || !Items.Contains(Item))
	{
		return false;
	}

	if (!Item->RemoveCount(Count))
	{
		return false;
	}
	
	if (Item->IsEmpty())
	{
		RemoveItemFromConsumableSlots(Item);

		if (WeaponSlot == Item)
		{
			WeaponSlot = nullptr;
		}
		
		Items.RemoveSingle(Item);
	}

	return true;
}

void UPlayerInventoryComponent::RemoveItemFromConsumableSlots(
	UItemInstance* Item)
{
	for (TObjectPtr<UItemInstance>& Slot : ConsumableSlots)
	{
		if (Slot == Item)
		{
			Slot = nullptr;
		}
	}
}

UItemInstance* UPlayerInventoryComponent::GetCurrentConsumable() const
{
	if(!ConsumableSlots.IsValidIndex(CurrentConsumableIndex))
	{
		return nullptr;
	}

	return ConsumableSlots[CurrentConsumableIndex];
}

bool UPlayerInventoryComponent::AddConsumableSlot(UItemInstance* Item)
{
	if (!Item)
	{
		return false;
	}

	// 인벤토리에 실제 존재하는 아이템인지 확인
	if (!Items.Contains(Item))
	{
		return false;
	}

	// 소비 아이템인지 확인
	if (!Cast<UConsumableDataAsset>(Item->GetItemData()))
	{
		return false;
	}

	// 이미 등록되어 있는지 확인
	if (ConsumableSlots.Contains(Item))
	{
		return false;
	}

	// 지금은 퀵슬롯 갯수 무한인데 추후 슬롯 수 제한가능
	ConsumableSlots.Add(Item);

	return true;
}

bool UPlayerInventoryComponent::RemoveConsumableSlot(UItemInstance* Item)
{
	return ConsumableSlots.RemoveSingle(Item) > 0;
}

void UPlayerInventoryComponent::SetConsumableSlot(
	int32 SlotIndex,
	UItemInstance* Item)
{
	if (!Items.Contains(Item))
	{
		return;
	}

	if (!Cast<UConsumableDataAsset>(Item->GetItemData()))
	{
		return;
	}


	if (ConsumableSlots.Num() <= SlotIndex)
	{
		ConsumableSlots.SetNum(SlotIndex + 1);
	}
	
	ConsumableSlots[SlotIndex] = Item;
}

bool UPlayerInventoryComponent::SetWeaponSlot(UItemInstance* Item)
{
	if (!Item)
	{
		return false;
	}

	if (!Items.Contains(Item))
	{
		return false;
	}

	if (!Cast<UWeaponDataAsset>(Item->GetItemData()))
	{
		return false;
	}

	WeaponSlot = Item;

	return true;
}

void UPlayerInventoryComponent::ClearWeaponSlot()
{
	WeaponSlot = nullptr;
}

UItemInstance* UPlayerInventoryComponent::GetCurrentWeapon() const
{
	return WeaponSlot;
}