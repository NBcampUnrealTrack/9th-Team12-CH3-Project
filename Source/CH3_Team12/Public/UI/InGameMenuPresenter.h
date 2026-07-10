#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "InGameMenuPresenter.generated.h"

class UInGameMenuWidget;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UInGameMenuPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	void Initialize(UInGameMenuWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UInGameMenuWidget> InGameMenuWidget;
};
