// ReSharper disable CppMemberFunctionMayBeConst
#include "UI/StageResultWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Framework/Subsystem/KatanaLevelSubsystem.h"
#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"

void UStageResultWidget::UpdateWidget(const float BestTime, const float ClearTime)
{
	if (BestTime > 0)
	{
		const int32 BestIntSeconds = FMath::FloorToInt(BestTime);
		const int32 BestMin = BestIntSeconds / 60;
		const int32 BestSec = BestIntSeconds % 60;
		const int32 BestMS = FMath::RoundToInt((BestTime - BestIntSeconds) * 1000.f);
		const FString BestTimeString = FString::Printf(TEXT("Best Time : %02d : %02d.%03d"), BestMin, BestSec, BestMS);

		TxtBestTime->SetText(FText::FromString(BestTimeString));
	}
	else
	{
		TxtBestTime->SetText(FText::FromString(TEXT("Best Time : -- : --.--")));
	}

	const int32 ClearIntSeconds = FMath::FloorToInt(ClearTime);
	const int32 ClearMin = ClearIntSeconds / 60;
	const int32 ClearSec = ClearIntSeconds % 60;
	const int32 ClearMS = FMath::RoundToInt((ClearTime - ClearIntSeconds) * 1000.f);

	const FString ClearTimeString =
		FString::Printf(TEXT("Clear Time :  %02d : %02d.%03d"), ClearMin, ClearSec, ClearMS);

	TxtClearTime->SetText(FText::FromString(ClearTimeString));
}

void UStageResultWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BtnDone->OnClicked.AddDynamic(this, &UStageResultWidget::HandleBtnDoneClicked);
}

void UStageResultWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UStageResultWidget::HandleBtnDoneClicked()
{
	(void)OnBtnDoneClicked.ExecuteIfBound();
}
