#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "KatanaLoadingPresenter.generated.h"

class UKatanaLevelSubsystem;
class UKatanaLoadingWidget;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UKatanaLoadingPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	void Initialize(UKatanaLevelSubsystem* InSubsystem, UKatanaLoadingWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UKatanaLevelSubsystem> LevelSubsystem;

	UPROPERTY()
	TWeakObjectPtr<UKatanaLoadingWidget> LoadingWidget;

	UFUNCTION()
	void OnModelProgressUpdated(float Percent) const;
};
