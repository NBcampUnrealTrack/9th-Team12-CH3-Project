#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "StepProgressBar.generated.h"

class UImage;
class UTextBlock;
class UHorizontalBox;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnValueChanged, float, NewValue);

/**
 *
 */
UCLASS()
class CH3_TEAM12_API UStepProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "ProgressBar")
	FOnValueChanged OnValueChanged;

	UFUNCTION(BlueprintCallable, Category = "ProgressBar")
	void SetPercent(float InPercent);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Img_Background;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UHorizontalBox> HBox_Widgets;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Txt_Display;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ProgressBar")
	float MinDisplayValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ProgressBar")
	float MaxDisplayValue = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ProgressBar")
	float ActiveOpacity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ProgressBar")
	float InactiveOpacity = 0.15f;

	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	float CurrentValue = 1.0f;
	bool bIsDragging = false;

	int32 GetStepCount() const;
	float NormalizeValue(const float InValue) const;
	bool ApplyValue(const float InValue, const bool bBroadcastChange);
	float CalculateValueFromMouse(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) const;
	void UpdateValueFromMouse(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	void UpdateVisuals() const;
};
