#include "UI/HUD/PlayerWidget.h"

#include "Components/Image.h"
#include "UI/Widget/CenterProgressBar.h"
#include "UI/Widget/DelayedProgressBar.h"

void UPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	OriginPostureColor = PostureBar->GetColorAndOpacity();
	OriginPostureCenterColor = PostureCenterImage->GetColorAndOpacity();
}

void UPlayerWidget::UpdateHealthBar(const float Percent)
{
	HealthBar->SetPercent(Percent);

	if (HealthPercent > Percent)
	{
		PostureBar->SetColorAndOpacity(DamagePostureColor);
		bTakeDamageHealth = true;
	}

	HealthPercent = Percent;
}

void UPlayerWidget::UpdatePostureBar(const float Percent)
{
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
