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
	
	// 이미 장착 중이면 해제
	if (EquippedWeaponItem)
	{
		Unequip();
		return;
	}
	
	if (!Inventory)
	{
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
	if (bIsChangingWeapon)
	{
		return false;
	}
	
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
	
	bIsChangingWeapon = true;
	
	PendingEquipItem = Item;

	StateComp->AddStateTag(CombatTags::State_Movement_Locked);
	
	OwnerCharacter->PlayAnimMontage(WeaponData->EquipMontage);

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
		return;
	}
	
	bIsChangingWeapon = false;
	
	EquipWeapon(PendingEquipItem, WeaponData);

	PendingEquipItem = nullptr;
	
	if (StateComp->HasStateTag(CombatTags::State_Movement_Locked))
	{
		StateComp->RemoveStateTag(CombatTags::State_Movement_Locked);
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

	bIsChangingWeapon = true;
	
	PendingUnequipItem = EquippedWeaponItem;

	StateComp->AddStateTag(CombatTags::State_Movement_Locked);
	
	OwnerCharacter->PlayAnimMontage(WeaponData->UnequipMontage);
}

void UPlayerEquipmentComponent::OnUnequipNotify()
{
	bIsChangingWeapon = false;
	
	UnequipWeapon();
	
	PendingUnequipItem = nullptr;
	
	if (StateComp->HasStateTag(CombatTags::State_Movement_Locked))
	{
		StateComp->RemoveStateTag(CombatTags::State_Movement_Locked);
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

	if (bIsChangingWeapon)
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

	if (StateComp->HasStateTag(CombatTags::State_Movement_Locked))
	{
		return false;
	}

	if (StateComp->HasStateTag(CombatTags::State_Hit_Dead))
	{
		return false;
	}

	return true;
}