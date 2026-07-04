#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "KatanaEnemyPresenter.generated.h"

class UKatanaEnemyWidget;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UKatanaEnemyPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	void Initialize(UObject* InAttributeComponent, UKatanaEnemyWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UObject> AttributeComponent; //Model

	UPROPERTY()
	TWeakObjectPtr<UKatanaEnemyWidget> EnemyWidget; //View

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
