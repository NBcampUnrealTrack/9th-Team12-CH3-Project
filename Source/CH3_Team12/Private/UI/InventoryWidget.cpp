#include "UI/InventoryWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
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

// ReSharper disable once CppMemberFunctionMayBeConst
void UInventoryWidget::UpdateDetailWidget(const FText& Text, UTexture2D* Icon, int32 Count, int32 MaxCount,
                                          const FText& Description)
{
	TxtDetailName->SetText(Text);
	ImgDetailIcon->SetBrushFromTexture(Icon);
	TxtDetailCapacity->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), Count, MaxCount)));
	TxtDetailDescription->SetText(Description);
}

void UInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}
