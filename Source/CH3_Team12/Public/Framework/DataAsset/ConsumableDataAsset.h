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

	UConsumableDataAsset()
	{
		MaxStack = 99;
	}
	
	/** 아이템 사용 시 실행될 Effect */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Consumable")
	TObjectPtr<UItemEffect> Effect;

	/** 사용 시간(0이면 즉시 사용) */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable", meta = (ClampMin = "0.0"))
	float UseTime = 0.0f;

	/** 사용 후 소비 여부 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable")
	bool bConsumeOnUse = true;
};
