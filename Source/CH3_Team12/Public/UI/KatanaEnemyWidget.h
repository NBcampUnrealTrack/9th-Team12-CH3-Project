#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KatanaEnemyWidget.generated.h"

class UCenterProgressBar;
class UProgressBar;
class UDelayedProgressBar;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UKatanaEnemyWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateHealthBar(float Percent) const;
	void UpdatePostureBar(float Percent) const;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDelayedProgressBar> HealthBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCenterProgressBar> PostureBar;
};
