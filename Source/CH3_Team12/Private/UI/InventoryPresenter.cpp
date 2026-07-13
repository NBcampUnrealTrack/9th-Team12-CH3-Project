#include "UI/InventoryPresenter.h"

#include "Engine/World.h"
#include "Entity/Item/ItemInstance.h"
#include "Entity/Player/PlayerAttributeComponent.h"
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

	if (const UPlayerInventoryComponent* PlayerInventoryComponent = GetPlayerInventoryComponent())
	{
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

			Widget->OnTextButtonClicked.BindDynamic(this, &UInventoryPresenter::HandleInventoryItemClicked);
		}
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

void UInventoryPresenter::HandleInventoryItemClicked(const FText& ItemName)
{
	const FName ItemNameName = FName(ItemName.ToString());
	if (!InventoryItemDataMap.Contains(ItemNameName))
		return;

	auto [Name, Icon, Count, MaxCount, Description] = InventoryItemDataMap[ItemNameName];
	InventoryWidget->UpdateDetailWidget(Name, Icon, Count, MaxCount, Description);
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
