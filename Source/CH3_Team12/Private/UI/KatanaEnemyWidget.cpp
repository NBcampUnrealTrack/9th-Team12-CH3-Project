#include "UI/KatanaEnemyWidget.h"

#include "Components/ProgressBar.h"
#include "UI/Widget/CenterProgressBar.h"
#include "UI/Widget/DelayedProgressBar.h"

void UKatanaEnemyWidget::UpdateHealthBar(const float Percent) const
{
	if (!HealthBar)
		return;
	HealthBar->SetPercent(Percent);
}

void UKatanaEnemyWidget::UpdatePostureBar(const float Percent) const
{
	if (!PostureBar)
		return;
	PostureBar->SetPercent(Percent);
}
