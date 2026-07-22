#pragma once

#include "CoreMinimal.h"
#include "UICommonTypes.h"
#include "Blueprint/UserWidget.h"
#include "StageResultWidget.generated.h"

class UImage;
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FLinearColor BackgroundColor;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ImgBackground;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TxtBestTime;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TxtClearTime;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnDone;

	UFUNCTION()
	void HandleBtnDoneClicked();
};
