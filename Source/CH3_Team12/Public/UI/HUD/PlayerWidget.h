#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

class UImage;
class UCenterProgressBar;
class UDelayedProgressBar;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	FLinearColor DamagePostureColor;

	void UpdateHealthBar(float Percent);
	void UpdatePostureBar(float Percent);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDelayedProgressBar> HealthBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCenterProgressBar> PostureBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> PostureCenterImage;

private:
	float HealthPercent;
	bool bTakeDamageHealth = false;

	float PosturePercent;

	FLinearColor OriginPostureColor;
	FLinearColor OriginPostureCenterColor;
};
