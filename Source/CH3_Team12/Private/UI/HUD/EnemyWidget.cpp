#include "UI/HUD/EnemyWidget.h"

#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "UI/Widget/CenterProgressBar.h"
#include "UI/Widget/DelayedProgressBar.h"

// ReSharper disable once CppMemberFunctionMayBeConst
void UEnemyWidget::UpdateName(const FString& InName)
{
	TxtName->SetText(FText::FromString(InName));
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UEnemyWidget::UpdateHealthBar(const float Percent)
{
	HealthBar->SetPercent(Percent);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UEnemyWidget::UpdatePostureBar(const float Percent)
{
	PostureBar->SetPercent(Percent);
}
