#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerWidget.generated.h"

class UTextBlock;
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FLinearColor NormalPostureColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FLinearColor DamagePostureColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FLinearColor BreakPostureColor;

	void UpdateHealthBar(float Percent);
	void UpdatePostureBar(float Percent);
	void BreakPostureBar();
	void RecoverPostureBar();

	void UpdateConsumable(UTexture2D* Texture, const FText& Text);
	void UpdateWeapon(UTexture2D* Texture);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDelayedProgressBar> HealthBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCenterProgressBar> PostureBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> PostureCenterImage;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ConsumableImage;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> ConsumableText;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> WeaponImage;

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	float HealthPercent;
	bool bTakeDamageHealth = false;

	float PosturePercent;

	FLinearColor OriginPostureCenterColor;
};
