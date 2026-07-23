#include "UI/InventoryPresenter.h"

#include "Engine/World.h"
#include "Entity/Item/ItemInstance.h"
#include "Entity/Player/PlayerAttributeComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerInventoryComponent.h"
#include "Entity/Player/PlayerItemUseComponent.h"
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

	if (UPlayerInventoryComponent* PlayerInventoryComponent = GetPlayerInventoryComponent())
	{
		const TArray<TObjectPtr<UItemInstance>>& Items = PlayerInventoryComponent->GetItems();

		InventoryWidget->ClearItemWidgets();
		InventoryItemDataMap.Empty();
		InventoryItemWidgetMap.Empty();

		for (auto Item : Items)
		{
			if (!Item || Item->IsEmpty())
				continue;

			const FPrimaryAssetId& ItemId = Item->GetItemData()->GetPrimaryAssetId();

			FInventoryItemData ItemData;
			ItemData.Name = Item->GetItemData()->ItemName;
			ItemData.Icon = Item->GetItemData()->Icon;
			ItemData.Count = Item->GetCount();
			ItemData.MaxCount = Item->GetItemData()->MaxStack;
			ItemData.Description = Item->GetItemData()->Description;
			ItemData.ItemInstance = Item;

			InventoryItemDataMap.Emplace(ItemId, ItemData);
		}

		for (auto InventoryItemData : InventoryItemDataMap)
		{
			UInventoryItemWidget* Widget = InventoryWidget->AddAndItemWidget();
			if (!Widget)
				continue;

			const FPrimaryAssetId& ItemId = InventoryItemData.Key;
			const FInventoryItemData& ItemDataValue = InventoryItemData.Value;
			Widget->UpdateData(ItemId, ItemDataValue.Name, ItemDataValue.Icon, ItemDataValue.Count);

			Widget->OnMouseLeftClicked.BindDynamic(this, &UInventoryPresenter::HandleInventoryItemLeftClicked);
			Widget->OnMouseRightClicked.BindDynamic(this, &UInventoryPresenter::HandleInventoryItemRightClicked);

			InventoryItemWidgetMap.Emplace(InventoryItemData.Key, Widget);
		}

		PlayerInventoryComponent->OnInventoryChanged.AddUObject(this, &UInventoryPresenter::HandleItemChanged);
	}

	if (UPlayerAttributeComponent* PlayerAttributeComponent = GetPlayerAttributeComponent())
	{
		PlayerAttributeComponent->OnHealthChanged.AddDynamic(this, &UInventoryPresenter::HandleModelHealthChanged);
		const float CurrentHealth = PlayerAttributeComponent->GetCurrentHealth();
		const float MaxHealth = PlayerAttributeComponent->GetMaxHealth();
		UpdateHealthBar(CurrentHealth, MaxHealth, true);
	}
}

void UInventoryPresenter::Dispose()
{
	if (UPlayerInventoryComponent* PlayerInventoryComponent = GetPlayerInventoryComponent())
	{
		PlayerInventoryComponent->OnInventoryChanged.RemoveAll(this);
	}

	if (UPlayerAttributeComponent* PlayerAttributeComponent = GetPlayerAttributeComponent())
	{
		PlayerAttributeComponent->OnHealthChanged.RemoveDynamic(this, &UInventoryPresenter::HandleModelHealthChanged);
	}
}

void UInventoryPresenter::HandleModelHealthChanged(const float CurrentHealth, const float MaxHealth)
{
	UpdateHealthBar(CurrentHealth, MaxHealth, false);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UInventoryPresenter::UpdateHealthBar(const float CurrentHealth, const float MaxHealth,
                                          const bool bImmediately)
{
	if (MaxHealth > 0.0f)
	{
		const float Percent = CurrentHealth / MaxHealth;
		InventoryWidget->UpdateHealthBar(Percent, bImmediately);
	}
}

void UInventoryPresenter::HandleInventoryItemLeftClicked(const FPrimaryAssetId& ItemId)
{
	if (!InventoryItemDataMap.Contains(ItemId))
		return;

	SelectedItemId = ItemId;

	const auto& ItemDataValue = InventoryItemDataMap[ItemId];
	InventoryWidget->UpdateDetailWidget(ItemDataValue.Name, ItemDataValue.Icon, ItemDataValue.Count,
	                                    ItemDataValue.MaxCount, ItemDataValue.Description);
}

void UInventoryPresenter::HandleInventoryItemRightClicked(const FPrimaryAssetId& ItemId)
{
	if (!InventoryItemDataMap.Contains(ItemId))
		return;

	UPlayerItemUseComponent* PlayerItemUseComponent = GetPlayerItemUseComponent();
	if (!PlayerItemUseComponent)
		return;

	PlayerItemUseComponent->UseItem(InventoryItemDataMap[ItemId].ItemInstance);
}

void UInventoryPresenter::HandleItemChanged(UItemInstance* ItemInstance)
{
	if (!ItemInstance || !ItemInstance->GetItemData()) return;

	const FPrimaryAssetId ItemId = ItemInstance->GetItemData()->GetPrimaryAssetId();

	FInventoryItemData* ItemDataPtr = InventoryItemDataMap.Find(ItemId);
	TObjectPtr<UInventoryItemWidget>* WidgetPtr = InventoryItemWidgetMap.Find(ItemId);

	if (!ItemDataPtr || !WidgetPtr || !*WidgetPtr) return;

	if (ItemInstance->GetCount() <= 0) // 0이하인 경우 삭제
	{
		(*WidgetPtr)->RemoveFromParent();

		InventoryItemDataMap.Remove(ItemId);
		InventoryItemWidgetMap.Remove(ItemId);
		return;
	}

	ItemDataPtr->Count = ItemInstance->GetCount();
	ItemDataPtr->ItemInstance = ItemInstance;

	(*WidgetPtr)->UpdateData(ItemId, ItemDataPtr->Name, ItemDataPtr->Icon, ItemDataPtr->Count);

	if (ItemId == SelectedItemId)
	{
		InventoryWidget->UpdateDetailWidget(ItemDataPtr->Name, ItemDataPtr->Icon, ItemDataPtr->Count,
										ItemDataPtr->MaxCount, ItemDataPtr->Description);
	}
}

UPlayerAttributeComponent* UInventoryPresenter::GetPlayerAttributeComponent() const
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

	return PlayerCharacter->GetAttributeComponent();
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

UPlayerItemUseComponent* UInventoryPresenter::GetPlayerItemUseComponent() const
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

	return PlayerCharacter->GetItemUseComponent();
}
