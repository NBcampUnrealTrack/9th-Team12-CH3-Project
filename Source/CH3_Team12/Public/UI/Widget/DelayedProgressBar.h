#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DelayedProgressBar.generated.h"

class UImage;
class UMaterialInstanceDynamic;

/**
 * 피해 및 회복 지연 효과가 적용되는 프로그레스 바
 */
UCLASS()
class CH3_TEAM12_API UDelayedProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "ProgressBar")
	float InterpSpeed = 5.0f;

	UPROPERTY(EditAnywhere, Category = "ProgressBar")
	float DelayTime = 0.3f;

	UFUNCTION(BlueprintCallable, Category = "ProgressBar")
	void SetPercent(float InPercent, bool bImmediately = false);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Background;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_DamageDelayed;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_RecoveryDelayed;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Img_Current;

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	float TargetPercent = 1.0f;
	float CurrentPercent = 1.0f;
	float DamageDelayedPercent = 1.0f;
	float DelayRemainingTime = 0.0f;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DamageDelayedMat;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> RecoveryDelayedMat;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CurrentMat;

	static void SetProgress(UMaterialInstanceDynamic* Material, float Percent);

	static void InterpolateProgress(float& Current, float Target, float DeltaTime, float InterpSpeed,
	                                UMaterialInstanceDynamic* Material
	);
};
