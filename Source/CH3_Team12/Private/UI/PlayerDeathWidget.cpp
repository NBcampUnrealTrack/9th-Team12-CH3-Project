// ReSharper disable CppMemberFunctionMayBeConst
#include "UI/PlayerDeathWidget.h"

#include "Components/Button.h"

void UPlayerDeathWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	BtnRestart->OnClicked.AddDynamic(this, &UPlayerDeathWidget::HandleRestartButtonClicked);
	BtnMainMenu->OnClicked.AddDynamic(this, &UPlayerDeathWidget::HandleMainMenuButtonClicked);
}

void UPlayerDeathWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UPlayerDeathWidget::HandleRestartButtonClicked()
{
	(void)OnRestartButtonClicked.ExecuteIfBound();
}

void UPlayerDeathWidget::HandleMainMenuButtonClicked()
{
	(void)OnMainMenuButtonClicked.ExecuteIfBound();
}
