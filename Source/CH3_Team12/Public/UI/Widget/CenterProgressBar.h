#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CenterProgressBar.generated.h"

class UImage;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UCenterProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "ProgressBar")
	void SetPercent(float InPercent) const;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Background;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Current;

	virtual void NativeConstruct() override;

private:
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CurrentMat;
};
