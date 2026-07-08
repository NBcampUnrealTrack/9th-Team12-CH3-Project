#pragma once

#include "CoreMinimal.h"
#include "Entity/Item/ItemEffect.h"
#include "HealHpEffect.generated.h"

UCLASS(BlueprintType, EditInlineNew, DefaultToInstanced)
class CH3_TEAM12_API UHealHpEffect : public UItemEffect
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Heal")
	float HealAmount = 50.f;

	virtual bool CanApply_Implementation(AActor* Target) const override;

	virtual void Apply_Implementation(AActor* Target) override;
};
