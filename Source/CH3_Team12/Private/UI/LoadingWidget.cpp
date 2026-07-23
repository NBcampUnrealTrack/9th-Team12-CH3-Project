#include "UI/LoadingWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void ULoadingWidget::UpdateWidget(UTexture2D* Texture, FText LoadingTip)
{
	ImgFront->SetBrushFromTexture(Texture);
	TxtLoadingTip->SetText(LoadingTip);
}

void ULoadingWidget::UpdateProgress(const float Percent)
{
	if (!ProgressBar)
		return;

	ProgressBar->SetPercent(Percent);
}
