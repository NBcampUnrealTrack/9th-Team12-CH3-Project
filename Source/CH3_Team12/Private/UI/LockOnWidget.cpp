// ReSharper disable CppMemberFunctionMayBeConst
#include "UI/LockOnWidget.h"

#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"

void ULockOnWidget::UpdateWidget(const FVector2D& ScreenPosition)
{
	if (!CanvasSlot)
		return;
	CanvasSlot->SetPosition(ScreenPosition);
}

void ULockOnWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CanvasSlot = Cast<UCanvasPanelSlot>(LockOnIcon->Slot);
}
