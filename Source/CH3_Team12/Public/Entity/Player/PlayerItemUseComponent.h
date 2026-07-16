#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerItemUseComponent.generated.h"

class UPlayerLocomotionComponent;
class UStateTagComponent;
struct FInputActionValue;
class UAnimMontage;
class APlayerCharacterBase;
class UPlayerInventoryComponent;
class UConsumableDataAsset;
class UItemInstance;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH3_TEAM12_API UPlayerItemUseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerItemUseComponent();

	// R 입력 바인딩
	void UseConsumableInput(const FInputActionValue& Value);

	// AnimNotify
	void AnimNotify_UseConsumable();

	void UseItem(UItemInstance* Item);

	float GetCurrentMoveSpeedMultiplier() const;
protected:
	virtual void BeginPlay() override;

private:
	void UseConsumable(
		UItemInstance* Item,
		const UConsumableDataAsset* ConsumableData);

	bool ApplyConsumableEffects(
		const UConsumableDataAsset* ConsumableData);

	void OnItemUseMontageEnded(
		UAnimMontage* Montage,
		bool bInterrupted);

	bool canUseItem();
private:

	UPROPERTY()
	TObjectPtr<UPlayerInventoryComponent> Inventory;

	UPROPERTY()
	TObjectPtr<UPlayerLocomotionComponent> Locomotion;

	UPROPERTY()
	TObjectPtr<UStateTagComponent> StateComp;

	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;


	UPROPERTY()
	TObjectPtr<UItemInstance> CurrentUsingItem;
};
