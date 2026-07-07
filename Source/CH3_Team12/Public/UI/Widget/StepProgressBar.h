#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "StepProgressBar.generated.h"

class UTextBlock;
class UHorizontalBox;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStepValueChanged, float, NewValue);

/**
 *
 */
UCLASS()
class CH3_TEAM12_API UStepProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "UI|CustomBar")
	FOnStepValueChanged OnStepValueChanged;

	UFUNCTION(BlueprintCallable, Category = "UI|CustomBar")
	void SetValue(float InValue);

	UFUNCTION(BlueprintPure, Category = "UI|CustomBar")
	float GetValue() const { return CurrentValue; }

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> GaugeBox;

	UPROPERTY(meta=(BindWidget, OptionalWidget = true))
	TObjectPtr<UTextBlock> GaugeTextBlock;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI|Settings")
	float MinValue = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI|Settings")
	float MaxValue = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI|Settings")
	float ActiveOpacity = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI|Settings")
	float InactiveOpacity = 0.15f;

	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	void UpdateValueFromMouse(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent);
	void UpdateVisuals() const;

	float CurrentValue = 1.0f;
	bool bIsDragging = false;
};
