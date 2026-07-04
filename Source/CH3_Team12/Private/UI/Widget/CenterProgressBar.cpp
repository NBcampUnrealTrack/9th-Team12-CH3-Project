#include "UI/Widget/CenterProgressBar.h"

#include "Components/Image.h"
#include "Materials/MaterialInstanceDynamic.h"

void UCenterProgressBar::NativeConstruct()
{
	Super::NativeConstruct();

	if (Img_Current)
	{
		CurrentMat = Img_Current->GetDynamicMaterial();
	}
}

void UCenterProgressBar::SetPercent(const float InPercent) const
{
	if (CurrentMat)
	{
		const float Percent = FMath::Clamp(InPercent, 0.0f, 1.0f);
		CurrentMat->SetScalarParameterValue(TEXT("Progress"), Percent);
	}
}
