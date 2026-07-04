#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DelayedProgressBar.generated.h"

class UImage;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UDelayedProgressBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "ProgressBar")
	float InterpSpeed = 5.0f; // 잔상이 따라오는 속도 (낮을수록 느림)

	UPROPERTY(EditAnywhere, Category = "ProgressBar")
	float DamageDelayTime = 0.3f; // 맞은 후 잔상이 멈춰있는 시간 (초)

	UPROPERTY(EditAnywhere, Category = "ProgressBar")
	float RecoveryDelayTime = 0.3f; // 회복 후 잔상이 멈춰있는 시간 (초)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ProgressBar")
	bool IsCumulative = true;

	UFUNCTION(BlueprintCallable, Category = "ProgressBar")
	void SetPercent(float InPercent);

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
	float TargetPercent = 1.0f; // 목표 체력 비율 (0.0 ~ 1.0)
	float CurrentPercent = 1.0f; // 현재 표시 중인 실제 체력 비율
	float DamageDelayedPercent = 1.0f; // 뒤따라오는 잔상 체력 비율
	float RecoveryDelayedPercent = 1.0f; // 뒤따라오는 잔상 체력 비율

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> DamageDelayedMat;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> RecoveryDelayedMat;
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> CurrentMat;

	FTimerHandle DamageDelayTimerHandle;
	bool bCanAnimateDamageDelayed = false;

	FTimerHandle RecoveryDelayTimerHandle;
	bool bCanAnimateCurrentRecovery = false;

	void StartDamageDelayedAnimation();
	void StartRecoveryDelayedAnimation();
};
