#include "UI/KatanaPlayerWidget.h"

#include "Components/ProgressBar.h"
#include "UI/Widget/CenterProgressBar.h"
#include "UI/Widget/DelayedProgressBar.h"

void UKatanaPlayerWidget::UpdateHealthBar(const float Percent) const
{
	if (!HealthBar)
		return;
	HealthBar->SetPercent(Percent);
}

void UKatanaPlayerWidget::UpdatePostureBar(const float Percent) const
{
	if (!PostureBar)
		return;
	PostureBar->SetPercent(Percent);
}
