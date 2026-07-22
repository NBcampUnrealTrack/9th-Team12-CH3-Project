// ReSharper disable CppMemberFunctionMayBeConst
#include "UI/PlayerDeathWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UPlayerDeathWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BtnRestart->OnClicked.AddDynamic(this, &UPlayerDeathWidget::HandleRestartButtonClicked);
	BtnMainMenu->OnClicked.AddDynamic(this, &UPlayerDeathWidget::HandleMainMenuButtonClicked);

	ImgBackground->SetColorAndOpacity(FLinearColor::Transparent);
}

void UPlayerDeathWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const FLinearColor CurrentTxtColor = TxtDisplay->GetColorAndOpacity().GetSpecifiedColor();
	const FLinearColor InterpolatedColor = FMath::CInterpTo(CurrentTxtColor, DisplayColor, InDeltaTime, 0.5f);
	TxtDisplay->SetColorAndOpacity(FSlateColor(InterpolatedColor));

	const FLinearColor CurrentColor = ImgBackground->GetColorAndOpacity();
	ImgBackground->SetColorAndOpacity(FMath::CInterpTo(CurrentColor, BackgroundColor, InDeltaTime, 0.5f));
}

void UPlayerDeathWidget::HandleRestartButtonClicked()
{
	(void)OnRestartButtonClicked.ExecuteIfBound();
}

void UPlayerDeathWidget::HandleMainMenuButtonClicked()
{
	(void)OnMainMenuButtonClicked.ExecuteIfBound();
}
