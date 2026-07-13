#include "UI/KatanaLoadingWidget.h"

#include "Components/ProgressBar.h"

void UKatanaLoadingWidget::UpdateProgress(const float Percent)
{
	if (!ProgressBar)
		return;

	ProgressBar->SetPercent(Percent);
}
