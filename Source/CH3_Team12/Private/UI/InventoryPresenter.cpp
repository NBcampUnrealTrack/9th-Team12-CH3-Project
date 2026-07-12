#include "UI/InventoryPresenter.h"

#include "Engine/World.h"
#include "Entity/Item/ItemInstance.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerInventoryComponent.h"
#include "Framework/DataAsset/ItemDataAsset.h"
#include "Framework/GameMode/KatanaPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "UI/InventoryItemWidget.h"
#include "UI/InventoryWidget.h"

void UInventoryPresenter::Initialize(UInventoryWidget* InWidget)
{
	InventoryWidget = InWidget;
	if (!InventoryWidget.IsValid())
		return;

	const UPlayerInventoryComponent* PlayerInventoryComponent = GetPlayerInventoryComponent();
	if (!PlayerInventoryComponent)
		return;

	const TArray<TObjectPtr<UItemInstance>>& Items = PlayerInventoryComponent->GetItems();

	InventoryWidget->ClearItemWidgets();
	InventoryItemDataMap.Empty();

	for (auto Item : Items)
	{
		if (!Item || Item->IsEmpty())
			continue;

		FName ItemKeyName = FName(*Item->GetItemData()->ItemName.ToString());

		FInventoryItemData ItemData;
		ItemData.Name = Item->GetItemData()->ItemName;
		ItemData.Icon = Item->GetItemData()->Icon;
		ItemData.Count = Item->GetCount();
		ItemData.MaxCount = Item->GetItemData()->MaxStack;
		ItemData.Description = Item->GetItemData()->Description;

		InventoryItemDataMap.Emplace(ItemKeyName, ItemData);
	}

	for (auto InventoryItemData : InventoryItemDataMap)
	{
		UInventoryItemWidget* Widget = InventoryWidget->AddAndItemWidget();
		if (!Widget)
			continue;

		auto [Name, Icon, Count, MaxCount, Description] = InventoryItemData.Value;
		Widget->UpdateData(Name, Icon, Count);
	}
}

void UInventoryPresenter::Dispose()
{
}

UPlayerInventoryComponent* UInventoryPresenter::GetPlayerInventoryComponent() const
{
	if (!GetWorld())
		return nullptr;

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
		return nullptr;

	const AKatanaPlayerController* KatanaPlayerController = Cast<AKatanaPlayerController>(PlayerController);
	if (!KatanaPlayerController)
		return nullptr;

	const APlayerCharacterBase* PlayerCharacter = KatanaPlayerController->GetPlayerCharacter();
	if (!PlayerCharacter)
		return nullptr;

	return PlayerCharacter->GetInventoryComponent();
}
