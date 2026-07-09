#include "Entity/Player/PlayerEquipmentComponent.h"
#include "Entity/Item/ItemInstance.h"
#include "Entity/Weapon/WeaponBase.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerInventoryComponent.h"
#include "Entity/Player/StateTagComponent.h"
#include "Framework/DataAsset/WeaponDataAsset.h"
#include "GameplayTags/CombatGameplayTags.h"
#include "Engine/World.h"
#include "Components/SkeletalMeshComponent.h"
#include "InputActionValue.h"

UPlayerEquipmentComponent::UPlayerEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UPlayerEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerCharacter = Cast<APlayerCharacterBase>(GetOwner());
	
	if (!OwnerCharacter)
	{
		return;
	}
	
	Inventory = OwnerCharacter->FindComponentByClass<UPlayerInventoryComponent>();
	
	StateComp = OwnerCharacter->FindComponentByClass<UStateTagComponent>();
}

void UPlayerEquipmentComponent::ToggleWeaponInput(
	const FInputActionValue& Value)
{
	if (!CanChangeWeapon())
	{
		return;
	}
	
	if (!Inventory)
	{
		return;
	}
	
	// 이미 장착 중이면 해제
	if (EquippedWeaponItem)
	{
		Unequip();
		return;
	}
	
	// 인벤토리의 현재 무기 슬롯 사용
	UItemInstance* WeaponItem = Inventory->GetCurrentWeapon();

	if (!WeaponItem)
	{
		return;
	}

	Equip(WeaponItem);
}

bool UPlayerEquipmentComponent::Equip(UItemInstance* Item)
{
	if (!OwnerCharacter || !Item || PendingEquipItem)
	{
		return false;
	}
	
	const UWeaponDataAsset* WeaponData =
		Cast<UWeaponDataAsset>(Item->GetItemData());

	if (!WeaponData)
	{
		return false;
	}
	
	PendingEquipItem = Item;
	
	StateComp->AddStateTag(CombatTags::State_Action_Equipping);
	
	const float Duration =
	OwnerCharacter->PlayAnimMontage(WeaponData->EquipMontage);

	if (Duration <= 0.f)
	{
		PendingEquipItem = nullptr;
		
		StateComp->RemoveStateTag(CombatTags::State_Action_Equipping);

		return false;
	}
	
	return true;
}

void UPlayerEquipmentComponent::OnEquipNotify()
{
	if (!PendingEquipItem)
	{
		return;
	}

	const UWeaponDataAsset* WeaponData =
		Cast<UWeaponDataAsset>(PendingEquipItem->GetItemData());

	if (!WeaponData)
	{
		PendingEquipItem = nullptr;
		
		StateComp->RemoveStateTag(CombatTags::State_Action_Equipping);
		
		return;
	}
	
	EquipWeapon(PendingEquipItem, WeaponData);

	PendingEquipItem = nullptr;
	
	if (StateComp->HasStateTag(CombatTags::State_Action_Equipping))
	{
		StateComp->RemoveStateTag(CombatTags::State_Action_Equipping);
	}
}

bool UPlayerEquipmentComponent::EquipWeapon(
	UItemInstance* Item,
	const UWeaponDataAsset* WeaponData)
{
	if (!OwnerCharacter)
	{
		return false;
	}

	if (EquippedWeapon)
	{
		UnequipWeapon();
	}
	
	if (!WeaponData || !WeaponData->WeaponClass)
	{
		return false;
	}
	
	FActorSpawnParameters Params;

	Params.Owner = OwnerCharacter;
	Params.Instigator = OwnerCharacter;
	
	AWeaponBase* NewWeapon =
	GetWorld()->SpawnActor<AWeaponBase>(
		WeaponData->WeaponClass,
		Params);

	if (!NewWeapon)
	{
		return false;
	}

	NewWeapon->AttachToComponent(
	OwnerCharacter->GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		WeaponData->EquipSocket);

	EquippedWeapon = NewWeapon;
	EquippedWeaponItem = Item;

	StateComp->AddStateTag(CombatTags::State_Combat_Armed);
	
	return true;
}

void UPlayerEquipmentComponent::Unequip()
{
	if (!EquippedWeaponItem)
	{
		return;
	}

	const UWeaponDataAsset* WeaponData =
		Cast<UWeaponDataAsset>(EquippedWeaponItem->GetItemData());

	if (!WeaponData)
	{
		return;
	}

	StateComp->AddStateTag(CombatTags::State_Action_Equipping);
	
	const float Duration = 
		OwnerCharacter->PlayAnimMontage(WeaponData->UnequipMontage);
	
	if (Duration <= 0.f)
	{
		StateComp->RemoveStateTag(CombatTags::State_Action_Equipping);
	}
}

void UPlayerEquipmentComponent::OnUnequipNotify()
{
	UnequipWeapon();
	
	if (StateComp->HasStateTag(CombatTags::State_Action_Equipping))
	{
		StateComp->RemoveStateTag(CombatTags::State_Action_Equipping);
	}
}

void UPlayerEquipmentComponent::UnequipWeapon()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
		EquippedWeapon = nullptr;
	}

	EquippedWeaponItem = nullptr;
	
	if (StateComp->HasStateTag(CombatTags::State_Combat_Armed))
	{
		StateComp->RemoveStateTag(CombatTags::State_Combat_Armed);
	}
}

bool UPlayerEquipmentComponent::CanChangeWeapon() const
{
	if (!OwnerCharacter || !Inventory || !StateComp)
	{
		return false;
	}

	if (StateComp->HasStateTag(CombatTags::State_Combat_Attacking))
	{
		return false;
	}

	if (StateComp->HasStateTag(CombatTags::State_Combat_Dodging))
	{
		return false;
	}

	if (StateComp->HasStateTag(CombatTags::State_Action_Equipping))
	{
		return false;
	}

	if (StateComp->HasStateTag(CombatTags::State_Hit_Dead))
	{
		return false;
	}

	return true;
}