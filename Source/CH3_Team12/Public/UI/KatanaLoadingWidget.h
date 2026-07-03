#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KatanaLoadingWidget.generated.h"

class UProgressBar;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UKatanaLoadingWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateProgress(float Percent);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;
};
