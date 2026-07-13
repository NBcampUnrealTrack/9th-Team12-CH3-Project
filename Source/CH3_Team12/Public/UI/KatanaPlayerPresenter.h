#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "KatanaPlayerPresenter.generated.h"

class UPlayerAttributeComponent;
class UKatanaPlayerWidget;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UKatanaPlayerPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	void Initialize(UPlayerAttributeComponent* InAttributeComponent, UKatanaPlayerWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UPlayerAttributeComponent> AttributeComponent; //Model

	UPROPERTY()
	TWeakObjectPtr<UKatanaPlayerWidget> PlayerWidget; //View

	UFUNCTION()
	void OnModelHealthChanged(float CurrentHealth, float MaxHealth) const;

	UFUNCTION()
	void OnModelPostureChanged(float CurrentPosture, float MaxPosture) const;

	UFUNCTION()
	void OnModelPostureBroken() const;

	UFUNCTION()
	void OnModelPostureRecovered() const;

	UFUNCTION()
	void OnModelDeath() const;
};
