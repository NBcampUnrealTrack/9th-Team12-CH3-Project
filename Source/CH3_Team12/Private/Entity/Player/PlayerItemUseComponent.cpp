#include "Entity/Player/PlayerItemUseComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerInventoryComponent.h"
#include "Framework/DataAsset/ItemDataAsset.h"
#include "Framework/DataAsset/ConsumableDataAsset.h"
#include "Entity/Item/ItemEffect.h"
#include "Entity/Item/ItemInstance.h"
#include "InputActionValue.h"

UPlayerItemUseComponent::UPlayerItemUseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UPlayerItemUseComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<APlayerCharacterBase>(GetOwner());

	Inventory =
		OwnerCharacter->FindComponentByClass<UPlayerInventoryComponent>();
}

void UPlayerItemUseComponent::UseConsumableInput(
	const FInputActionValue& Value)
{
	if (!Inventory)
	{
		return;
	}

	if (!Inventory->GetCurrentConsumable())
	{
		return;
	}

	OwnerCharacter->PlayAnimMontage(UseConsumableMontage);
}

void UPlayerItemUseComponent::AnimNotify_UseConsumable()
{
	if (!Inventory)
	{
		return;
	}

	UseItem(Inventory->GetCurrentConsumable());
}

void UPlayerItemUseComponent::UseItem(UItemInstance* Item)
{
	if (!Item)
	{
		return;
	}

	if (const UConsumableDataAsset* Consumable =
		Cast<UConsumableDataAsset>(Item->GetItemData()))
	{
		UseConsumable(Item, Consumable);
	}
}

void UPlayerItemUseComponent::UseConsumable(
	UItemInstance* Item,
	const UConsumableDataAsset* ConsumableData)
{
	if (!Item || !ConsumableData)
	{
		return;
	}

	if(!ApplyConsumableEffects(ConsumableData))
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Item Use %s, Item Count : %d"), 
			*ConsumableData->ItemName.ToString(),
			Item->GetCount());
	
	if (ConsumableData->bConsumeOnUse)
	{
		Inventory->RemoveItem(Item, 1);
	}
}

bool UPlayerItemUseComponent::ApplyConsumableEffects(
	const UConsumableDataAsset* ConsumableData)
{
	if (!ConsumableData)
	{
		return false;
	}

	AActor* OwnerActor = GetOwner();

	for (UItemEffect* Effect : ConsumableData->Effects)
	{
		if (!Effect)
		{
			continue;
		}

		if (!Effect->CanApply(OwnerActor))
		{
			return false;
		}
	}

	for (UItemEffect* Effect : ConsumableData->Effects)
	{
		if (!Effect)
		{
			continue;
		}

		Effect->Apply(OwnerActor);
	}
	
	return true;
}