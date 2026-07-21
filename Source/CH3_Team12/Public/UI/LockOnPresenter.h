#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "LockOnPresenter.generated.h"

class ULockOnWidget;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API ULockOnPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	void Initialize(AActor* InTargetActor, ULockOnWidget* InWidget);
	virtual void Dispose() override;

	virtual bool IsTickable() const override { return bCanTick; }
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	TWeakObjectPtr<ULockOnWidget> LockOnWidget;

	UPROPERTY()
	TWeakObjectPtr<AActor> TargetActor;

	UPROPERTY()
	TWeakObjectPtr<APlayerController> CachedPlayerController;

	bool bCanTick = false;

	FVector TargetOffset;
};
