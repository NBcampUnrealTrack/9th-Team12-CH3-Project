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
