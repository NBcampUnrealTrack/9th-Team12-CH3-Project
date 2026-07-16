#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyWidget.generated.h"

class UImage;
class UTextBlock;
class UCenterProgressBar;
class UProgressBar;
class UDelayedProgressBar;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UEnemyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FLinearColor NormalPostureColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FLinearColor DamagePostureColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FLinearColor BreakPostureColor;

	void UpdateName(const FString& InName);
	void UpdateHealthBar(float Percent);
	void UpdatePostureBar(float Percent);
	void BreakPostureBar();
	void RecoverPostureBar();

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TxtName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDelayedProgressBar> HealthBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCenterProgressBar> PostureBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> PostureCenterImage;

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	float HealthPercent;
	bool bTakeDamageHealth = false;

	float PosturePercent;

	FLinearColor OriginPostureCenterColor;
};
