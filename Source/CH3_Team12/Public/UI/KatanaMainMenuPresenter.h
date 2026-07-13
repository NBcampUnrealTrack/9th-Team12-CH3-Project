#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "KatanaMainMenuPresenter.generated.h"

class UKatanaMainMenuWidget;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UKatanaMainMenuPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	void Initialize(UKatanaMainMenuWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UKatanaMainMenuWidget> MainMenuWidget;

	UFUNCTION()
	void OnPlayButtonClicked() const;

	UFUNCTION()
	void OnSettingsButtonClicked() const;

	UFUNCTION()
	void OnQuitButtonClicked() const;
};
