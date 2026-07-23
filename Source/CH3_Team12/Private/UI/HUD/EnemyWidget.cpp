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
	bTakeDamageHealth = HealthPercent > Percent;
	HealthPercent = Percent;
}

void UEnemyWidget::UpdatePostureBar(const float Percent)
{
	PostureBar->SetPercent(Percent);
	PosturePercent = Percent;
}

void UEnemyWidget::BreakPostureBar()
{
	bBreakPosture = true;
}

void UEnemyWidget::RecoverPostureBar()
{
	bBreakPosture = false;
}

void UEnemyWidget::NativeConstruct()
{
	Super::NativeConstruct();
	OriginPostureCenterColor = PostureCenterImage->GetColorAndOpacity();
}

void UEnemyWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (PosturePercent > 0.0f)
	{
		FLinearColor PostureColor = NormalPostureColor;
		if (bTakeDamageHealth)
			PostureColor = DamagePostureColor;
		else if (bBreakPosture)
			PostureColor = BreakPostureColor;

		PostureBar->SetColorAndOpacity(PostureColor);
		PostureCenterImage->SetColorAndOpacity(OriginPostureCenterColor);
	}
	else
	{
		PostureBar->SetColorAndOpacity(FLinearColor::Transparent);
		PostureCenterImage->SetColorAndOpacity(FLinearColor::Transparent);
		bTakeDamageHealth = false;
	}
}
