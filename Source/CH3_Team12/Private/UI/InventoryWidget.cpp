#include "UI/InventoryWidget.h"

#include "Components/VerticalBox.h"
#include "UI/InventoryItemWidget.h"
#include "UI/Widget/DelayedProgressBar.h"

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

// ReSharper disable once CppMemberFunctionMayBeConst
void UInventoryWidget::ClearItemWidgets()
{
	VerticalBox->ClearChildren();
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UInventoryWidget::UpdateHealthBar(const float Percent, const bool bImmediately)
{
	HealthBar->SetPercent(Percent, bImmediately);
}

void UInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}
