#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "InventoryPresenter.generated.h"

class UInventoryWidget;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UInventoryPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	virtual void Initialize(UInventoryWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UInventoryWidget> InventoryWidget;
};
