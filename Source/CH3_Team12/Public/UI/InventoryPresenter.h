#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "InventoryPresenter.generated.h"

class UInventoryItemWidget;
class UItemInstance;
class UPlayerItemUseComponent;
class UPlayerAttributeComponent;
class UPlayerInventoryComponent;
class UTexture2D;
class UInventoryWidget;

USTRUCT(BlueprintType)
struct CH3_TEAM12_API FInventoryItemData
{
	GENERATED_BODY()

	UPROPERTY()
	FText Name;

	UPROPERTY()
	UTexture2D* Icon;

	UPROPERTY()
	int32 Count;

	UPROPERTY()
	int32 MaxCount;

	UPROPERTY()
	FText Description;

	UPROPERTY()
	TObjectPtr<UItemInstance> ItemInstance;
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
	TWeakObjectPtr<UInventoryWidget> InventoryWidget;

	UPROPERTY()
	TMap<FPrimaryAssetId, FInventoryItemData> InventoryItemDataMap;

	UPROPERTY()
	TMap<FPrimaryAssetId, TObjectPtr<UInventoryItemWidget>> InventoryItemWidgetMap;

	FPrimaryAssetId SelectedItemId;

	UFUNCTION()
	void HandleModelHealthChanged(float CurrentHealth, float MaxHealth);

	UFUNCTION()
	void UpdateHealthBar(float CurrentHealth, float MaxHealth, bool bImmediately = false);

	UFUNCTION()
	void HandleInventoryItemLeftClicked(const FPrimaryAssetId& ItemId);

	UFUNCTION()
	void HandleInventoryItemRightClicked(const FPrimaryAssetId& ItemId);

	UFUNCTION()
	void HandleItemChanged(UItemInstance* ItemInstance);

	UFUNCTION()
	UPlayerAttributeComponent* GetPlayerAttributeComponent() const;

	UFUNCTION()
	UPlayerInventoryComponent* GetPlayerInventoryComponent() const;

	UFUNCTION()
	UPlayerItemUseComponent* GetPlayerItemUseComponent() const;
};
