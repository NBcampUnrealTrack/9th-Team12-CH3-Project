#include "Entity/Item/HealHpEffect.h"
#include "Entity/Player/PlayerAttributeComponent.h"
#include "GameFramework/Actor.h"

bool UHealHpEffect::CanApply_Implementation(AActor* Target) const
{
	if (!Target)
	{
		return false;
	}

	return Target->FindComponentByClass<UPlayerAttributeComponent>() != nullptr;
}

void UHealHpEffect::Apply_Implementation(AActor* Target)
{
	if (!Target)
	{
		return;
	}

	UPlayerAttributeComponent* AttributeComponent = 
		Target->FindComponentByClass<UPlayerAttributeComponent>();

	if (!AttributeComponent)
	{
		return;
	}

	AttributeComponent->Heal(HealAmount);
}
