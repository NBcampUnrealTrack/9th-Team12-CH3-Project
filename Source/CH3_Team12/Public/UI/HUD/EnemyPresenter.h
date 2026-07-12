#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "EnemyPresenter.generated.h"

class UEnemyAttributeComponent;
class UEnemyWidget;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UEnemyPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	void Initialize(const FString& InName, UEnemyAttributeComponent* InAttributeComponent, UEnemyWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UEnemyAttributeComponent> AttributeComponent; //Model

	UPROPERTY()
	TWeakObjectPtr<UEnemyWidget> EnemyWidget; //View

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
