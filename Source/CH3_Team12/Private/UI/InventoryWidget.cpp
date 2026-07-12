#include "UI/InventoryWidget.h"

#include "Components/VerticalBox.h"
#include "UI/InventoryItemWidget.h"

UInventoryItemWidget* UInventoryWidget::AddAndItemWidget()
{
	if (!VerticalBox || !ItemWidget) return nullptr;

	UInventoryItemWidget* NewSlot = CreateWidget<UInventoryItemWidget>(this, ItemWidget);
	if (NewSlot)
	{
		VerticalBox->AddChildToVerticalBox(NewSlot);
	}
	return NewSlot;
}

void UInventoryWidget::ClearItemWidgets()
{
	VerticalBox->ClearChildren();
}

void UInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}
