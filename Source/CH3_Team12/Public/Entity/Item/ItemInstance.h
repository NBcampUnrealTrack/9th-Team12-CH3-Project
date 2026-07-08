#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemInstance.generated.h"

class UItemDataAsset;

UCLASS(BlueprintType)
class CH3_TEAM12_API UItemInstance : public UObject
{
	GENERATED_BODY()
	
public:
	void Initialize(UItemDataAsset* InItemData, int32 InCount = 1);

	const UItemDataAsset* GetItemData() const { return ItemData; }

	int32 GetCount() const { return Count; }

	void AddCount(int32 Amount);

	bool RemoveCount(int32 Amount);

	bool IsEmpty() const { return Count <= 0; }
	
private:
	/** 아이템 데이터 */
	UPROPERTY()
	TObjectPtr<UItemDataAsset> ItemData;

	/** 현재 스택 개수 */
	UPROPERTY()
	int32 Count = 1;
	
};
