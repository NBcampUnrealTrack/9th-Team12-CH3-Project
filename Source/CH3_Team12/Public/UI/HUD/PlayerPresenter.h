#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "PlayerPresenter.generated.h"

class UPlayerAttributeComponent;
class UPlayerWidget;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UPlayerPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	void Initialize(UPlayerAttributeComponent* InAttributeComponent, UPlayerWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UPlayerAttributeComponent> AttributeComponent; //Model

	UPROPERTY()
	TWeakObjectPtr<UPlayerWidget> PlayerWidget; //View

	UFUNCTION()
	void HandleModelHealthChanged(float CurrentHealth, float MaxHealth);

	UFUNCTION()
	void HandleModelPostureChanged(float CurrentPosture, float MaxPosture);

	UFUNCTION()
	void HandleModelPostureBroken();

	UFUNCTION()
	void HandleModelPostureRecovered();

	UFUNCTION()
	void HandleModelDeath();
};
