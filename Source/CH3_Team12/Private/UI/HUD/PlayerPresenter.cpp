// ReSharper disable CppMemberFunctionMayBeConst
#include "UI/HUD/PlayerPresenter.h"

#include "Entity/Item/ItemInstance.h"
#include "Entity/Player/PlayerAttributeComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerInventoryComponent.h"
#include "Framework/DataAsset/ItemDataAsset.h"
#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"
#include "UI/HUD/PlayerWidget.h"

void UPlayerPresenter::Initialize(const APlayerCharacterBase* InPlayerCharacterBase, UPlayerWidget* InWidget)
{
	if (!InPlayerCharacterBase || !InWidget)
		return;

	PlayerWidget = InWidget;

	AttributeComponent = InPlayerCharacterBase->GetAttributeComponent();
	if (AttributeComponent.IsValid())
	{
		AttributeComponent->OnHealthChanged.AddDynamic(this, &UPlayerPresenter::HandleModelHealthChanged);
		AttributeComponent->OnPostureChanged.AddDynamic(this, &UPlayerPresenter::HandleModelPostureChanged);
		AttributeComponent->OnPostureBroken.AddDynamic(this, &UPlayerPresenter::HandleModelPostureBroken);
		AttributeComponent->OnPostureRecovered.AddDynamic(this, &UPlayerPresenter::HandleModelPostureRecovered);
		AttributeComponent->OnDead.AddDynamic(this, &UPlayerPresenter::HandleModelDeath);

		HandleModelHealthChanged(AttributeComponent->GetCurrentHealth(), AttributeComponent->GetMaxHealth());
		HandleModelPostureChanged(AttributeComponent->GetCurrentPosture(), AttributeComponent->GetMaxPosture());
	}

	InventoryComponent = InPlayerCharacterBase->GetInventoryComponent();
	if (InventoryComponent.IsValid())
	{
		InventoryComponent->OnInventoryChanged.AddUObject(this, &UPlayerPresenter::HandleModelInventoryChanged);
		InventoryComponent->OnConsumableSlotChanged.AddUObject(this, &UPlayerPresenter::HandleModelConsumableSlotChanged);
		InventoryComponent->OnWeaponSlotChanged.AddUObject(this, &UPlayerPresenter::HandleModelWeaponSlotChanged);
	}
}

void UPlayerPresenter::Dispose()
{
	if (AttributeComponent.IsValid())
	{
		AttributeComponent->OnHealthChanged.RemoveDynamic(this, &UPlayerPresenter::HandleModelHealthChanged);
		AttributeComponent->OnPostureChanged.RemoveDynamic(this, &UPlayerPresenter::HandleModelPostureChanged);
		AttributeComponent->OnPostureBroken.RemoveDynamic(this, &UPlayerPresenter::HandleModelPostureBroken);
		AttributeComponent->OnPostureRecovered.RemoveDynamic(this, &UPlayerPresenter::HandleModelPostureRecovered);
		AttributeComponent->OnDead.RemoveDynamic(this, &UPlayerPresenter::HandleModelDeath);
	}

	if (InventoryComponent.IsValid())
	{
		InventoryComponent->OnConsumableSlotChanged.RemoveAll(this);
		InventoryComponent->OnWeaponSlotChanged.RemoveAll(this);
	}
}

void UPlayerPresenter::HandleModelHealthChanged(const float CurrentHealth, const float MaxHealth)
{
	if (PlayerWidget.IsValid() && MaxHealth > 0.0f)
	{
		const float Percent = CurrentHealth / MaxHealth;
		PlayerWidget->UpdateHealthBar(Percent);
	}
}

void UPlayerPresenter::HandleModelPostureChanged(const float CurrentPosture, const float MaxPosture)
{
	if (PlayerWidget.IsValid() && MaxPosture > 0.0f)
	{
		const float Percent = CurrentPosture / MaxPosture;
		PlayerWidget->UpdatePostureBar(Percent);
	}
}

void UPlayerPresenter::HandleModelPostureBroken()
{
	if (PlayerWidget.IsValid())
	{
		PlayerWidget->BreakPostureBar();
	}
}

void UPlayerPresenter::HandleModelPostureRecovered()
{
	if (PlayerWidget.IsValid())
	{
		PlayerWidget->RecoverPostureBar();
	}
}

void UPlayerPresenter::HandleModelDeath()
{
	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(this);
	if (!UIManager)
		return;

	UIManager->HidePlayerWidget();
}

void UPlayerPresenter::HandleModelInventoryChanged(UItemInstance* Item)
{
	if (PlayerWidget.IsValid() && ConsumableItem == Item)
	{
		UTexture2D* Texture2D = Item->GetItemData()->Icon;
		const FText Text = FText::AsNumber(Item->GetCount());
		PlayerWidget->UpdateConsumable(Texture2D, Text);
	}
}

void UPlayerPresenter::HandleModelConsumableSlotChanged(int32 SlotIndex, UItemInstance* Item)
{
	if (PlayerWidget.IsValid() && Item)
	{
		UTexture2D* Texture2D = Item->GetItemData()->Icon;
		const FText Text = FText::AsNumber(Item->GetCount());
		PlayerWidget->UpdateConsumable(Texture2D, Text);

		ConsumableItem = Item;
	}
}

void UPlayerPresenter::HandleModelWeaponSlotChanged(UItemInstance* Item)
{
	if (PlayerWidget.IsValid() && Item)
	{
		UTexture2D* Texture2D = Item->GetItemData()->Icon;
		PlayerWidget->UpdateWeapon(Texture2D);
	}
}
