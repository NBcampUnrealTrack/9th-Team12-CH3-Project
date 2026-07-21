#pragma once

#include "CoreMinimal.h"
#include "UICommonTypes.h"
#include "Blueprint/UserWidget.h"
#include "StageResultWidget.generated.h"

class UButton;
class UTextBlock;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UStageResultWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnButtonClicked OnBtnDoneClicked;

	void UpdateWidget(float BestTime, float ClearTime);

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TxtBestTime;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TxtClearTime;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnDone;

	UFUNCTION()
	void HandleBtnDoneClicked();
};
