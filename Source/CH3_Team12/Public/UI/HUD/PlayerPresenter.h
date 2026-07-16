#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "PlayerPresenter.generated.h"

class UItemInstance;
class UPlayerInventoryComponent;
class APlayerCharacterBase;
class UPlayerAttributeComponent;
class UPlayerWidget;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UPlayerPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	void Initialize(const APlayerCharacterBase* InPlayerCharacterBase, UPlayerWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UPlayerAttributeComponent> AttributeComponent;

	UPROPERTY()
	TWeakObjectPtr<UPlayerInventoryComponent> InventoryComponent;

	UPROPERTY()
	TWeakObjectPtr<UPlayerWidget> PlayerWidget; //View

	UPROPERTY()
	TWeakObjectPtr<UItemInstance> ConsumableItem;

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

	UFUNCTION()
	void HandleModelInventoryChanged(UItemInstance* Item);

	UFUNCTION()
	void HandleModelConsumableSlotChanged(int32 SlotIndex, UItemInstance* Item);

	UFUNCTION()
	void HandleModelWeaponSlotChanged(UItemInstance* Item);
};
