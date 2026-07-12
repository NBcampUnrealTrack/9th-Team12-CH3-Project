#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyWidget.generated.h"

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
	void UpdateName(const FString& InName);
	void UpdateHealthBar(float Percent);
	void UpdatePostureBar(float Percent);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TxtName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UDelayedProgressBar> HealthBar;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCenterProgressBar> PostureBar;
};
