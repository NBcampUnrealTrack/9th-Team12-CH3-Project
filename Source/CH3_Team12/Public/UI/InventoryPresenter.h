#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "InventoryPresenter.generated.h"

class UPlayerAttributeComponent;
class UPlayerInventoryComponent;
class UTexture2D;
class UInventoryWidget;

USTRUCT(BlueprintType)
struct CH3_TEAM12_API FInventoryItemData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Count;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxCount;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;
};

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

	UPROPERTY()
	TMap<FName, FInventoryItemData> InventoryItemDataMap;

	UFUNCTION()
	void HandleModelHealthChanged(float CurrentHealth, float MaxHealth);

	UFUNCTION()
	UPlayerAttributeComponent* GetPlayerAttributeComponent() const;

	UFUNCTION()
	UPlayerInventoryComponent* GetPlayerInventoryComponent() const;
};
