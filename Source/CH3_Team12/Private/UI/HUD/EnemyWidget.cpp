// ReSharper disable CppMemberFunctionMayBeConst
#include "UI/HUD/EnemyWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "UI/Widget/CenterProgressBar.h"
#include "UI/Widget/DelayedProgressBar.h"

void UEnemyWidget::UpdateName(const FString& InName)
{
	TxtName->SetText(FText::FromString(InName));
}

void UEnemyWidget::UpdateHealthBar(const float Percent)
{
	HealthBar->SetPercent(Percent);

	if (HealthPercent > Percent)
	{
		PostureBar->SetColorAndOpacity(DamagePostureColor);
		bTakeDamageHealth = true;
	}
	else if (HealthPercent < Percent)
	{
		PostureBar->SetColorAndOpacity(NormalPostureColor);
		bTakeDamageHealth = false;
	}

	HealthPercent = Percent;
}

void UEnemyWidget::UpdatePostureBar(const float Percent)
{
	FLinearColor PostureColor = NormalPostureColor;
	if (bTakeDamageHealth)
		PostureColor = DamagePostureColor;

	PostureBar->SetColorAndOpacity(PostureColor);
	PostureCenterImage->SetColorAndOpacity(OriginPostureCenterColor);

	PostureBar->SetPercent(Percent);

	PosturePercent = Percent;
}

void UEnemyWidget::BreakPostureBar()
{
	PostureBar->SetColorAndOpacity(BreakPostureColor);
}

void UEnemyWidget::RecoverPostureBar()
{
	PostureBar->SetColorAndOpacity(NormalPostureColor);
}

void UEnemyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	OriginPostureCenterColor = PostureCenterImage->GetColorAndOpacity();
}

void UEnemyWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (PosturePercent <= 0.0f)
	{
		PostureBar->SetColorAndOpacity(FLinearColor::Transparent);
		PostureCenterImage->SetColorAndOpacity(FLinearColor::Transparent);
		bTakeDamageHealth = false;
	}
}
