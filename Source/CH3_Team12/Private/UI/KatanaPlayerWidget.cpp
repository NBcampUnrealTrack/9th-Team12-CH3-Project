// ReSharper disable CppMemberFunctionMayBeConst
#include "UI/KatanaPlayerWidget.h"

#include "Components/ProgressBar.h"
#include "UI/Widget/DelayedProgressBar.h"

void UKatanaPlayerWidget::UpdateHealthBar(const float Percent)
{
	if (!HealthBar)
		return;
	HealthBar->SetPercent(Percent);
}

void UKatanaPlayerWidget::UpdatePostureBar(const float Percent)
{
	if (!PostureBar)
		return;
	PostureBar->SetPercent(Percent);
}
