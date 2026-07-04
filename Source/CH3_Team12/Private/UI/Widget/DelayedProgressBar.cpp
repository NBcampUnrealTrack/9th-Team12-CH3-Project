#include "UI/Widget/DelayedProgressBar.h"

#include "TimerManager.h"
#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

void UDelayedProgressBar::NativeConstruct()
{
	Super::NativeConstruct();

	if (Img_DamageDelayed)
	{
		DamageDelayedMat = Img_DamageDelayed->GetDynamicMaterial();
		if (DamageDelayedMat)
		{
			DamageDelayedMat->SetScalarParameterValue(TEXT("Progress"), DamageDelayedPercent);
		}
	}

	if (Img_RecoveryDelayed)
	{
		RecoveryDelayedMat = Img_RecoveryDelayed->GetDynamicMaterial();
		if (RecoveryDelayedMat)
		{
			RecoveryDelayedMat->SetScalarParameterValue(TEXT("Progress"), RecoveryDelayedPercent);
		}
	}

	if (Img_Current)
	{
		CurrentMat = Img_Current->GetDynamicMaterial();
		if (CurrentMat)
		{
			CurrentMat->SetScalarParameterValue(TEXT("Progress"), CurrentPercent);
		}
	}
}

void UDelayedProgressBar::SetPercent(const float InPercent)
{
	const float PrePercent = CurrentPercent;
	const float NewPercent = FMath::Clamp(InPercent, 0.0f, 1.0f);

	TargetPercent = NewPercent;

	const bool bIsDamaged = NewPercent < PrePercent;
	const bool bIsRecovered = NewPercent > PrePercent;

	if (bIsDamaged)
	{
		CurrentPercent = NewPercent;

		DamageDelayedPercent = IsCumulative
			? FMath::Max(DamageDelayedPercent, PrePercent)
			: PrePercent;

		if (DamageDelayedMat)
		{
			DamageDelayedMat->SetScalarParameterValue(TEXT("Progress"), DamageDelayedPercent);
		}

		RecoveryDelayedPercent = CurrentPercent;
		if (RecoveryDelayedMat)
		{
			RecoveryDelayedMat->SetScalarParameterValue(TEXT("Progress"), RecoveryDelayedPercent);
		}

		if (CurrentMat)
		{
			CurrentMat->SetScalarParameterValue(TEXT("Progress"), CurrentPercent);
		}

		bCanAnimateDamageDelayed = false;
		bCanAnimateCurrentRecovery = false;
	}

	if (bIsRecovered)
	{
		RecoveryDelayedPercent = NewPercent;

		if (RecoveryDelayedMat)
		{
			RecoveryDelayedMat->SetScalarParameterValue(TEXT("Progress"), RecoveryDelayedPercent);
		}

		DamageDelayedPercent = NewPercent;
		if (DamageDelayedMat)
		{
			DamageDelayedMat->SetScalarParameterValue(TEXT("Progress"), DamageDelayedPercent);
		}

		if (CurrentMat)
		{
			CurrentMat->SetScalarParameterValue(TEXT("Progress"), CurrentPercent);
		}

		bCanAnimateDamageDelayed = false;
		bCanAnimateCurrentRecovery = false;
	}

	if (!GetWorld()) return;

	if (bIsDamaged)
	{
		GetWorld()->GetTimerManager().ClearTimer(DamageDelayTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(
			DamageDelayTimerHandle,
			this,
			&UDelayedProgressBar::StartDamageDelayedAnimation,
			DamageDelayTime,
			false
		);

		GetWorld()->GetTimerManager().ClearTimer(RecoveryDelayTimerHandle);
	}

	if (bIsRecovered)
	{
		GetWorld()->GetTimerManager().ClearTimer(RecoveryDelayTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(
			RecoveryDelayTimerHandle,
			this,
			&UDelayedProgressBar::StartRecoveryDelayedAnimation,
			RecoveryDelayTime,
			false
		);

		GetWorld()->GetTimerManager().ClearTimer(DamageDelayTimerHandle);
	}
}

void UDelayedProgressBar::StartDamageDelayedAnimation()
{
	bCanAnimateDamageDelayed = true;
}

void UDelayedProgressBar::StartRecoveryDelayedAnimation()
{
	bCanAnimateCurrentRecovery = true;
}

void UDelayedProgressBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bCanAnimateCurrentRecovery && CurrentPercent < TargetPercent)
	{
		CurrentPercent = FMath::FInterpTo(CurrentPercent, TargetPercent, InDeltaTime, InterpSpeed);

		if (FMath::IsNearlyEqual(CurrentPercent, TargetPercent, 0.001f))
		{
			CurrentPercent = TargetPercent;
			bCanAnimateCurrentRecovery = false;
		}

		if (CurrentMat)
		{
			CurrentMat->SetScalarParameterValue(TEXT("Progress"), CurrentPercent);
		}
	}

	if (bCanAnimateDamageDelayed && DamageDelayedPercent > TargetPercent)
	{
		DamageDelayedPercent = FMath::FInterpTo(DamageDelayedPercent, TargetPercent, InDeltaTime, InterpSpeed);

		if (FMath::IsNearlyEqual(DamageDelayedPercent, TargetPercent, 0.001f))
		{
			DamageDelayedPercent = TargetPercent;
			bCanAnimateDamageDelayed = false;
		}

		if (DamageDelayedMat)
		{
			DamageDelayedMat->SetScalarParameterValue(TEXT("Progress"), DamageDelayedPercent);
		}
	}
}