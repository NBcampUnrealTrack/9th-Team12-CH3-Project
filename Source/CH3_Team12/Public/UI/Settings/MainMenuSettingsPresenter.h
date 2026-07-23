#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "MainMenuSettingsPresenter.generated.h"

class UMainMenuSettingsWidget;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UMainMenuSettingsPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void Initialize(UMainMenuSettingsWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UMainMenuSettingsWidget> MainMenuSettingsWidget;

	UFUNCTION()
	void HandleBtnResetStageRecordClicked();

	UFUNCTION()
	void HandleBtnBackClicked();
};
