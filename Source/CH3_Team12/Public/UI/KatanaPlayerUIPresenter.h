// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "KatanaPlayerUIPresenter.generated.h"

class UKatanaPlayerWidget;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UKatanaPlayerUIPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	void Initialize(TScriptInterface<IExampleModelInterface> InModel, UKatanaPlayerWidget* InView);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TScriptInterface<IExampleModelInterface> Model;

	TWeakObjectPtr<UKatanaPlayerWidget> View;

	UFUNCTION()
	void OnModelHealthChanged(float CurrentHealth, float MaxHealth);

};
