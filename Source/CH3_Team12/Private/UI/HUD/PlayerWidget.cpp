// ReSharper disable CppMemberFunctionMayBeConst
#include "UI/HUD/PlayerWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "UI/Widget/CenterProgressBar.h"
#include "UI/Widget/DelayedProgressBar.h"

void UPlayerWidget::UpdateHealthBar(const float Percent)
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

void UPlayerWidget::UpdatePostureBar(const float Percent)
{
	FLinearColor PostureColor = NormalPostureColor;
	if (bTakeDamageHealth)
		PostureColor = DamagePostureColor;

	PostureBar->SetColorAndOpacity(PostureColor);
	PostureCenterImage->SetColorAndOpacity(OriginPostureCenterColor);

	PostureBar->SetPercent(Percent);

	PosturePercent = Percent;
}

void UPlayerWidget::BreakPostureBar()
{
	PostureBar->SetColorAndOpacity(BreakPostureColor);
}

void UPlayerWidget::RecoverPostureBar()
{
	PostureBar->SetColorAndOpacity(NormalPostureColor);
}

void UPlayerWidget::UpdateConsumable(UTexture2D* Texture, const FText& Text)
{
	ConsumableImage->SetBrushFromTexture(Texture);
	ConsumableText->SetText(Text);
}

void UPlayerWidget::UpdateWeapon(UTexture2D* Texture)
{
	WeaponImage->SetBrushFromTexture(Texture);
}

void UPlayerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	OriginPostureCenterColor = PostureCenterImage->GetColorAndOpacity();
}

void UPlayerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (PosturePercent <= 0.0f)
	{
		PostureBar->SetColorAndOpacity(FLinearColor::Transparent);
		PostureCenterImage->SetColorAndOpacity(FLinearColor::Transparent);
		bTakeDamageHealth = false;
	}
}
