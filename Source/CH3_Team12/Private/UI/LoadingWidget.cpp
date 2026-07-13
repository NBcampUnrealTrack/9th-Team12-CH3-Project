#include "UI/LoadingWidget.h"

#include "Components/ProgressBar.h"

void ULoadingWidget::UpdateProgress(const float Percent)
{
	if (!ProgressBar)
		return;

	ProgressBar->SetPercent(Percent);
}
