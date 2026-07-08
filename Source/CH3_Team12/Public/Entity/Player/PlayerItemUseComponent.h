#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerItemUseComponent.generated.h"


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
		
protected:
	virtual void BeginPlay() override;
	
private:

	void UseItem(UItemInstance* Item);

	void UseConsumable(
		UItemInstance* Item,
		const UConsumableDataAsset* ConsumableData);

	bool ApplyConsumableEffects(
		const UConsumableDataAsset* ConsumableData);
	
private:

	UPROPERTY()
	TObjectPtr<UPlayerInventoryComponent> Inventory;

	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;

	// 포션 사용 모션
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> UseConsumableMontage;
};
