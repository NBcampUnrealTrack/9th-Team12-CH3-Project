#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "LoadingPresenter.generated.h"

class UKatanaLevelSubsystem;
class ULoadingWidget;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API ULoadingPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	void Initialize(ULoadingWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UKatanaLevelSubsystem> LevelSubsystem;

	UPROPERTY()
	TWeakObjectPtr<ULoadingWidget> LoadingWidget;

	UFUNCTION()
	void OnModelProgressUpdated(float Percent) const;
};
