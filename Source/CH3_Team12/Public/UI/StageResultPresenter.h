#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "StageResultPresenter.generated.h"

class UStageResultWidget;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UStageResultPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	void Initialize(UStageResultWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UStageResultWidget> StageResultWidget;

	UFUNCTION()
	void HandleBtnDoneClicked();
};
