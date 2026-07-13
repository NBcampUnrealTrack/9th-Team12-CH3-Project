#include "UI/Widget/DelayedProgressBar.h"

#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

void UDelayedProgressBar::NativeConstruct()
{
	Super::NativeConstruct();

	DamageDelayedMat = Img_DamageDelayed->GetDynamicMaterial();
	RecoveryDelayedMat = Img_RecoveryDelayed->GetDynamicMaterial();
	CurrentMat = Img_Current->GetDynamicMaterial();

	SetProgress(DamageDelayedMat, DamageDelayedPercent);
	SetProgress(RecoveryDelayedMat, CurrentPercent);
	SetProgress(CurrentMat, CurrentPercent);
}

void UDelayedProgressBar::SetPercent(const float InPercent, const bool bImmediately)
{
	const float NewPercent = FMath::Clamp(InPercent, 0.0f, 1.0f);

	if (bImmediately)
	{
		TargetPercent = NewPercent;
		CurrentPercent = NewPercent;
		DamageDelayedPercent = NewPercent;
		DelayRemainingTime = 0.0f;

		SetProgress(DamageDelayedMat, NewPercent);
		SetProgress(RecoveryDelayedMat, NewPercent);
		SetProgress(CurrentMat, NewPercent);
		return;
	}

	const float PreviousPercent = CurrentPercent;

	if (FMath::IsNearlyEqual(NewPercent, PreviousPercent))
		return;

	TargetPercent = NewPercent;
	DelayRemainingTime = FMath::Max(DelayTime, 0.0f);

	if (NewPercent < PreviousPercent)
	{
		DamageDelayedPercent = FMath::Max(DamageDelayedPercent, PreviousPercent);

		CurrentPercent = NewPercent;

		SetProgress(DamageDelayedMat, DamageDelayedPercent);
		SetProgress(RecoveryDelayedMat, CurrentPercent);
		SetProgress(CurrentMat, CurrentPercent);
	}
	else
	{
		DamageDelayedPercent = NewPercent;

		SetProgress(DamageDelayedMat, DamageDelayedPercent);
		SetProgress(RecoveryDelayedMat, NewPercent);
	}
}

void UDelayedProgressBar::NativeTick(const FGeometry& MyGeometry, const float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (DelayRemainingTime > 0.0f)
	{
		DelayRemainingTime -= InDeltaTime;
		return;
	}

	if (CurrentPercent < TargetPercent)
	{
		InterpolateProgress(CurrentPercent, TargetPercent, InDeltaTime, InterpSpeed, CurrentMat);
	}
	else if (DamageDelayedPercent > TargetPercent)
	{
		InterpolateProgress(DamageDelayedPercent, TargetPercent, InDeltaTime, InterpSpeed, DamageDelayedMat);
	}
}

void UDelayedProgressBar::SetProgress(UMaterialInstanceDynamic* Material, const float Percent)
{
	if (Material)
	{
		Material->SetScalarParameterValue(TEXT("Progress"), Percent);
	}
}

void UDelayedProgressBar::InterpolateProgress(float& Current, const float Target, const float DeltaTime,
                                              const float InterpSpeed, UMaterialInstanceDynamic* Material)
{
	Current = FMath::FInterpTo(Current, Target, DeltaTime, InterpSpeed);

	if (FMath::IsNearlyEqual(Current, Target, 0.001f))
	{
		Current = Target;
	}

	SetProgress(Material, Current);
}
