#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "PlayerDeathPresenter.generated.h"

class UPlayerDeathWidget;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UPlayerDeathPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	void Initialize(UPlayerDeathWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UPlayerDeathWidget> PlayerDeathWidget;

	UFUNCTION()
	void HandleRestartButtonClicked();

	UFUNCTION()
	void HandleMainMenuButtonClicked();
};
