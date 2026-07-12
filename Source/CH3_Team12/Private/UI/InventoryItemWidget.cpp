#include "UI/InventoryItemWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

// ReSharper disable once CppMemberFunctionMayBeConst
void UInventoryItemWidget::SetInfo(UTexture2D* Texture, const int32 Count, const FString& Name)
{
	ImgIcon->SetBrushFromTexture(Texture);
	TxtCount->SetText(FText::Format(FText::FromString(TEXT("{0}")), Count));
	TxtName->SetText(FText::FromString(Name));
}
