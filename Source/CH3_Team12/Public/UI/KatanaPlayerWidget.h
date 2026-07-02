#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KatanaPlayerWidget.generated.h"

class UDelayedProgressBar;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UKatanaPlayerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateHealthBar(float HealthPercent);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDelayedProgressBar> CustomHealthBar;
};
