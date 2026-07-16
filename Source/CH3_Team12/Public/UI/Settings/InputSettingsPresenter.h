#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "InputSettingsPresenter.generated.h"

struct FKey;
class UInputSettingsWidget;
class UKatanaInputManagerSubsystem;

UCLASS()
class CH3_TEAM12_API UInputSettingsPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	virtual void Initialize(UInputSettingsWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UKatanaInputManagerSubsystem> InputManagerSubsystem;

	UPROPERTY()
	TWeakObjectPtr<UInputSettingsWidget> InputSettingsWidget;

	TMap<FName, FKey> PendingKeyBindings;

	UFUNCTION()
	void HandleKeyBindingChanged(FName ActionName, FKey NewKey);

	UFUNCTION()
	void HandleBtnResetClicked();

	UFUNCTION()
	void HandleBtnDoneClicked();
};