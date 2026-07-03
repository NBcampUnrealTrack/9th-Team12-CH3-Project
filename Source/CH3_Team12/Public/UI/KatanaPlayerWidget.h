#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KatanaPlayerWidget.generated.h"

class UProgressBar;
class UDelayedProgressBar;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UKatanaPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateHealthBar(float Percent);
	void UpdatePostureBar(float Percent);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDelayedProgressBar> HealthBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDelayedProgressBar> PostureBar;
};
