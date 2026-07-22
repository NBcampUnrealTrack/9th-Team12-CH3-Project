#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LoadingWidget.generated.h"

class UTextBlock;
class UImage;
class UProgressBar;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API ULoadingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateWidget(UTexture2D* Texture, FText LoadingTip);
	void UpdateProgress(float Percent);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ImgFront;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TxtLoadingTip;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;
};
