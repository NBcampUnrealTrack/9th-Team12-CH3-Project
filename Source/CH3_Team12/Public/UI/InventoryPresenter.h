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
	FText Name = FText::GetEmpty();

	UPROPERTY()
	UTexture2D* Icon = nullptr;

	UPROPERTY()
	int32 Count = 0;

	UPROPERTY()
	int32 MaxCount = 0;

	UPROPERTY()
	FText Description = FText::GetEmpty();

	UPROPERTY()
	TObjectPtr<UItemInstance> ItemInstance = nullptr;
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
