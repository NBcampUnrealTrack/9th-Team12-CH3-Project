#include "UI/KatanaPlayerWidget.h"

#include "Components/Image.h"
#include "UI/Widget/CenterProgressBar.h"
#include "UI/Widget/DelayedProgressBar.h"

void UKatanaPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (PostureBar)
		OriginPostureColor = PostureBar->GetColorAndOpacity();

	// TODO 아래 기능을 활용해 최대값에 맞춰 UI 사이즈 조정
	// auto* CanvasPanelSlot = Cast<UCanvasPanelSlot>(PostureBar->Slot);
	// CanvasPanelSlot->SetSize(FVector2D(1000.0f, 20.0f));

	if (PostureCenterImage)
		OriginPostureCenterColor = PostureCenterImage->GetColorAndOpacity();
}

void UKatanaPlayerWidget::UpdateHealthBar(const float Percent)
{
	if (!HealthBar)
		return;
	HealthBar->SetPercent(Percent);

	if (HealthPercent > Percent)
	{
		PostureBar->SetColorAndOpacity(DamagePostureColor);
		bTakeDamageHealth = true;
	}

	HealthPercent = Percent;
}

void UKatanaPlayerWidget::UpdatePostureBar(const float Percent)
{
	if (!PostureBar)
		return;

	PostureBar->SetColorAndOpacity(bTakeDamageHealth ? DamagePostureColor : OriginPostureColor);
	PostureCenterImage->SetColorAndOpacity(OriginPostureCenterColor);

	PostureBar->SetPercent(Percent);

	if (Percent <= 0.0f)
	{
		PostureBar->SetColorAndOpacity(FLinearColor::Transparent);
		PostureCenterImage->SetColorAndOpacity(FLinearColor::Transparent);
		bTakeDamageHealth = false;
	}
}
