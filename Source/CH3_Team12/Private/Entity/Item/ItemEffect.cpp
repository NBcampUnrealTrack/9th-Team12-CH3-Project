#include "Entity/Item/ItemEffect.h"

bool UItemEffect::CanApply_Implementation(AActor* Target) const
{
	return Target != nullptr;
}

void UItemEffect::Apply_Implementation(AActor* Target)
{
}
