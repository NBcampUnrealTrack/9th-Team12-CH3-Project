#include "UI/InventoryItemWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

// ReSharper disable once CppMemberFunctionMayBeConst
void UInventoryItemWidget::UpdateData(const FText& Name, UTexture2D* Icon, const int32 Count)
{
	TxtName->SetText(Name);
	ImgIcon->SetBrushFromTexture(Icon);
	TxtCount->SetText(FText::Format(FText::FromString(TEXT("{0}")), Count));
}

FReply UInventoryItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	const FKey PressedKey = InMouseEvent.GetEffectingButton();

	if (PressedKey == EKeys::LeftMouseButton)
	{
		HandleLeftClick();
		return FReply::Handled();
	}

	if (PressedKey == EKeys::RightMouseButton)
	{
		HandleRightClick();

		return FReply::Handled();
	}

	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UInventoryItemWidget::HandleLeftClick()
{
	(void)OnTextButtonClicked.ExecuteIfBound(TxtName->GetText());
}

void UInventoryItemWidget::HandleRightClick()
{
	UE_LOG(LogTemp, Log, TEXT("UKatanaOptionItemWidget: 우클릭 감지됨 -> 이전 옵션으로 변경"));
}
