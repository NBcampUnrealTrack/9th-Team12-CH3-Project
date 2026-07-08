#include "UI/HUD/EnemyWidget.h"

#include "Components/ProgressBar.h"
#include "UI/Widget/CenterProgressBar.h"
#include "UI/Widget/DelayedProgressBar.h"

void UEnemyWidget::UpdateHealthBar(const float Percent) const
{
	if (!HealthBar)
		return;
	HealthBar->SetPercent(Percent);
}

void UEnemyWidget::UpdatePostureBar(const float Percent) const
{
	if (!PostureBar)
		return;
	PostureBar->SetPercent(Percent);
}
