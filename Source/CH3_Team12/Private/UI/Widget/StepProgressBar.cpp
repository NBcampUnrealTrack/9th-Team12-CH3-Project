#include "UI/Widget/StepProgressBar.h"

#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

void UStepProgressBar::SetValue(float InValue)
{
    CurrentValue = FMath::GridSnap(FMath::Clamp(InValue, 0.0f, 1.0f), 0.1f);
    UpdateVisuals();
}

void UStepProgressBar::NativeConstruct()
{
    UpdateVisuals();
}

FReply UStepProgressBar::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
    {
        bIsDragging = true;
        UpdateValueFromMouse(InGeometry, InMouseEvent);
        return FReply::Handled().CaptureMouse(TakeWidget()); // 마우스 캡처 활성화
    }
    return FReply::Unhandled();
}

FReply UStepProgressBar::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && bIsDragging)
    {
        bIsDragging = false;
        return FReply::Handled().ReleaseMouseCapture(); // 마우스 캡처 해제
    }
    return FReply::Unhandled();
}

FReply UStepProgressBar::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (bIsDragging)
    {
        UpdateValueFromMouse(InGeometry, InMouseEvent);
        return FReply::Handled();
    }
    return FReply::Unhandled();
}

void UStepProgressBar::UpdateValueFromMouse(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    if (!GaugeBox) return;

    // 1. 위젯의 로컬 좌표계 기준, 현재 마우스의 X 위치값을 구합니다.
    const FVector2D LocalMousePos = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
    const float MouseX = LocalMousePos.X;

    // 2. 위젯의 전체 가로 폭 길이를 구합니다.
    const float TotalWidth = MyGeometry.GetLocalSize().X;
    if (TotalWidth <= 0.0f) return;

    // 3. 가로폭 대비 마우스 위치 비율을 구하고(0.0 ~ 1.0), 0.1 단위로 자석 스냅(GridSnap)을 겁니다.
    const float RawPercent = FMath::Clamp(MouseX / TotalWidth, 0.0f, 1.0f);
    const float SnappedPercent = FMath::GridSnap(RawPercent, 0.1f);

    // 4. 값이 변했을 때만 델리게이트를 발동시켜 전역 사운드 매니저를 때려줍니다.
    if (!FMath::IsNearlyEqual(CurrentValue, SnappedPercent))
    {
        CurrentValue = SnappedPercent;
        UpdateVisuals();

        OnStepValueChanged.Broadcast(CurrentValue);
    }
}

void UStepProgressBar::UpdateVisuals() const
{
    if (!GaugeBox) return;

    const int32 TotalSteps = GaugeBox->GetChildrenCount();
    if (TotalSteps <= 0) return;

    // 0.1 단위에 맞춰 켜야 할 정수 칸수를 구합니다.
    const int32 ActiveCount = FMath::RoundToInt(CurrentValue * TotalSteps);

    for (int32 i = 0; i < TotalSteps; ++i)
    {
        if (const TObjectPtr<UWidget> ChildWidget = GaugeBox->GetChildAt(i))
        {
            ChildWidget->SetRenderOpacity(i < ActiveCount ? ActiveOpacity : InactiveOpacity);
        }
    }

    if (GaugeTextBlock)
    {
        const float CurrentValuePercent = FMath::Lerp(MinValue, MaxValue, CurrentValue);
        const int32 DisplayValue = FMath::RoundToInt(CurrentValuePercent);

        GaugeTextBlock->SetText(FText::FromString(FString::FromInt(DisplayValue)));
    }
}
