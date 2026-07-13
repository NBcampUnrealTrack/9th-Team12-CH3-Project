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

	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerItemUseComponent : OwnerCharacter is nullptr"));
		return;
	}
	
	Inventory = OwnerCharacter->GetInventoryComponent();
	
	if (!Inventory)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerItemUseComponent : Inventory is nullptr"));
		return;
	}
}

void UPlayerItemUseComponent::UseConsumableInput(
	const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerItemUseComponent : Press button"));
	
	if (!Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerItemUseComponent : Inventory is nullptr"));
		return;
	}
	
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerItemUseComponent : OwnerCharacter is nullptr"));
		return;
	}

	if (!Inventory->GetCurrentConsumable())
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerItemUseComponent : CurrentConsumable is nullptr"));
		return;
	}
	
	if (!Inventory->GetCurrentConsumable()->GetItemData())
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerItemUseComponent : ConsumableData is nullptr"));
		return;
	}
	
	OwnerCharacter->PlayAnimMontage(
		Inventory->GetCurrentConsumable()->GetItemData()->UseMontage
		);
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