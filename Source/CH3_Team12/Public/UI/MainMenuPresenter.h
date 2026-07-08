#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "MainMenuPresenter.generated.h"

class UMainMenuWidget;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UMainMenuPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	void Initialize(UMainMenuWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UMainMenuWidget> MainMenuWidget;

	UFUNCTION()
	void OnPlayButtonClicked() const;

	UFUNCTION()
	void OnSettingsButtonClicked() const;

	UFUNCTION()
	void OnQuitButtonClicked() const;
};
