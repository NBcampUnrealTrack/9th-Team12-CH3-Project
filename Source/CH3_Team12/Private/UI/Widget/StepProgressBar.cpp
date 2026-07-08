#include "UI/Widget/StepProgressBar.h"

#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"

void UStepProgressBar::NativeConstruct()
{
    Super::NativeConstruct();
    UpdateVisuals();
}

FReply UStepProgressBar::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton)
        return FReply::Unhandled();

    bIsDragging = true;
    UpdateValueFromMouse(InGeometry, InMouseEvent);
    return FReply::Handled().CaptureMouse(TakeWidget()); // 마우스 캡처 활성화
}

FReply UStepProgressBar::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (InMouseEvent.GetEffectingButton() != EKeys::LeftMouseButton || !bIsDragging)
        return FReply::Unhandled();

    bIsDragging = false;
    return FReply::Handled().ReleaseMouseCapture(); // 마우스 캡처 해제
}

FReply UStepProgressBar::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (!bIsDragging)
        return FReply::Unhandled();

    UpdateValueFromMouse(InGeometry, InMouseEvent);
    return FReply::Handled();
}

void UStepProgressBar::SetPercent(const float InPercent)
{
    ApplyValue(InPercent, false);
}

int32 UStepProgressBar::GetStepCount() const
{
    return HBox_Widgets ? HBox_Widgets->GetChildrenCount() : 0;
}

float UStepProgressBar::NormalizeValue(const float InValue) const
{
    const int32 StepCount = GetStepCount();
    if (StepCount <= 0)
    {
        return FMath::Clamp(InValue, 0.0f, 1.0f);
    }

    const float ClampedValue = FMath::Clamp(InValue, 0.0f, 1.0f);
    const int32 ActiveStepCount = FMath::RoundToInt(ClampedValue * StepCount);

    return static_cast<float>(ActiveStepCount) / static_cast<float>(StepCount);
}

float UStepProgressBar::CalculateValueFromMouse(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) const
{
    const float TotalWidth = MyGeometry.GetLocalSize().X;
    if (TotalWidth <= 0.0f)
    {
        return CurrentValue;
    }

    const FVector2D LocalMousePosition = MyGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
    const float RawPercent = LocalMousePosition.X / TotalWidth;

    return NormalizeValue(RawPercent);
}

bool UStepProgressBar::ApplyValue(const float InValue, const bool bBroadcastChange)
{
    const float NewValue = NormalizeValue(InValue);
    if (FMath::IsNearlyEqual(CurrentValue, NewValue))
    {
        return false;
    }

    CurrentValue = NewValue;
    UpdateVisuals();

    if (bBroadcastChange)
    {
        OnValueChanged.Broadcast(CurrentValue);
    }

    return true;
}

void UStepProgressBar::UpdateValueFromMouse(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
    const float NewValue = CalculateValueFromMouse(MyGeometry, MouseEvent);
    ApplyValue(NewValue, true);
}

void UStepProgressBar::UpdateVisuals() const
{
    if (!HBox_Widgets) return;

    const int32 TotalSteps = GetStepCount();
    if (TotalSteps <= 0)
    {
        return;
    }

    const int32 ActiveCount = FMath::RoundToInt(CurrentValue * TotalSteps);

    for (int32 Index = 0; Index < TotalSteps; ++Index)
    {
        if (UWidget* ChildWidget = HBox_Widgets->GetChildAt(Index))
        {
            ChildWidget->SetRenderOpacity(Index < ActiveCount ? ActiveOpacity : InactiveOpacity);
        }
    }

    if (Txt_Display)
    {
        const float DisplayPercent = FMath::Lerp(MinDisplayValue, MaxDisplayValue, CurrentValue);
        const int32 DisplayValue = FMath::RoundToInt(DisplayPercent);

        Txt_Display->SetText(FText::AsNumber(DisplayValue));
    }
}
