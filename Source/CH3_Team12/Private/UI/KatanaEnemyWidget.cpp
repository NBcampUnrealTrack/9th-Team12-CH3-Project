// ReSharper disable CppMemberFunctionMayBeConst
#include "UI/KatanaEnemyWidget.h"

#include "Components/ProgressBar.h"
#include "UI/Widget/DelayedProgressBar.h"

void UKatanaEnemyWidget::UpdateHealthBar(const float Percent)
{
	if (!HealthBar)
		return;
	HealthBar->SetPercent(Percent);
}

void UKatanaEnemyWidget::UpdatePostureBar(const float Percent)
{
	if (!PostureBar)
		return;
	PostureBar->SetPercent(Percent);
}
