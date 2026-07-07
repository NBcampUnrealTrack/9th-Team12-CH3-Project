#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ItemEffect.generated.h"

class APlayerCharacterBase;
class UItemInstance;

UCLASS(Abstract, BlueprintType, EditInlineNew, DefaultToInstanced)
class CH3_TEAM12_API UItemEffect : public UObject
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintNativeEvent, Category = "Item Effect")
	bool CanApply(AActor* Target) const;

	virtual bool CanApply_Implementation(AActor* Target) const;

	UFUNCTION(BlueprintNativeEvent, Category = "Item Effect")
	void Apply(AActor* Target);

	virtual void Apply_Implementation(AActor* Target);
};
