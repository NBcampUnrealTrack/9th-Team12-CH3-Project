#include "Entity/Item/ItemInstance.h"
#include "Framework/DataAsset/ItemDataAsset.h"

void UItemInstance::Initialize(UItemDataAsset* InItemData, int32 InCount)
{
	ItemData = InItemData;
	Count = FMath::Max(InCount, 1);
}

void UItemInstance::AddCount(int32 Amount)
{
	if(Amount <= 0)
	{
		return;
	}

	Count += Amount;
}

bool UItemInstance::RemoveCount(int32 Amount)
{
	if (Amount <= 0)
	{
		return false;
	}

	if (Count < Amount)
	{
		return false;
	}

	Count -= Amount;

	return true;
}