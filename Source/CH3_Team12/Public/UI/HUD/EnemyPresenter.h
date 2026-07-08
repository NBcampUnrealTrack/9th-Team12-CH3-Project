#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "EnemyPresenter.generated.h"

class UEnemyWidget;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UEnemyPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	void Initialize(UObject* InAttributeComponent, UEnemyWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UObject> AttributeComponent; //Model

	UPROPERTY()
	TWeakObjectPtr<UEnemyWidget> EnemyWidget; //View

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
