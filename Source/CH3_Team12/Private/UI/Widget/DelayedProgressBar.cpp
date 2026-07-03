#include "UI/Widget/DelayedProgressBar.h"

#include "TimerManager.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

void UDelayedProgressBar::NativeConstruct()
{
	Super::NativeConstruct();

	if (Img_Current) CurrentMat = Img_Current->GetDynamicMaterial();
	if (Img_Delayed) DelayedMat = Img_Delayed->GetDynamicMaterial();
}

void UDelayedProgressBar::SetPercent(const float InPercent)
{
	const float PrePercent = CurrentPercent;

	TargetPercent = FMath::Clamp(InPercent, 0.0f, 1.0f);
	CurrentPercent = TargetPercent;

	if (!IsCumulative)
	{
		DelayedPercent = PrePercent;
		if (DelayedMat) DelayedMat->SetScalarParameterValue(TEXT("Progress"), DelayedPercent);
	}

	if (DelayedPercent < CurrentPercent)
	{
		DelayedPercent = CurrentPercent;
		if (DelayedMat) DelayedMat->SetScalarParameterValue(TEXT("Progress"), DelayedPercent);
	}

	if (CurrentMat)
	{
		CurrentMat->SetScalarParameterValue(TEXT("Progress"), CurrentPercent);
	}

	bCanAnimateDelayed = false;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(DelayTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(
			DelayTimerHandle,
			this,
			&UDelayedProgressBar::StartDelayedAnimation,
			DamageDelayTime,
			false
		);
	}
}

void UDelayedProgressBar::StartDelayedAnimation()
{
	bCanAnimateDelayed = true;
}

void UDelayedProgressBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bCanAnimateDelayed && DelayedPercent > TargetPercent)
	{
		DelayedPercent = FMath::FInterpTo(DelayedPercent, TargetPercent, InDeltaTime, InterpSpeed);

		if (FMath::IsNearlyEqual(DelayedPercent, TargetPercent, 0.001f))
		{
			DelayedPercent = TargetPercent;
			bCanAnimateDelayed = false;
		}

		if (DelayedMat)
		{
			DelayedMat->SetScalarParameterValue(TEXT("Progress"), DelayedPercent);
		}
	}
}