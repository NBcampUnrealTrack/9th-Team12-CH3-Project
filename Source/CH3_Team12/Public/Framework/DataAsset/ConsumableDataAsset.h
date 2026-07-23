#pragma once

#include "CoreMinimal.h"
#include "Framework/DataAsset/ItemDataAsset.h"
#include "ConsumableDataAsset.generated.h"

class UItemEffect;

UCLASS(BlueprintType)
class CH3_TEAM12_API UConsumableDataAsset : public UItemDataAsset
{
	GENERATED_BODY()
	
public:

	UConsumableDataAsset() { MaxStack = 99; }
	
	/** 사용 시 적용될 Effect 목록 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category="Consumable")
	TArray<TObjectPtr<UItemEffect>> Effects;
	
	/** 사용 후 소비 여부 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable")
	bool bConsumeOnUse = true;
	
};
