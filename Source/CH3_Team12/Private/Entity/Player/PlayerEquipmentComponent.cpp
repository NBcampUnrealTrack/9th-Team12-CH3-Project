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
#include "Animation/AnimInstance.h"
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
	
	if (!Inventory)
	{
		return;
	}
	
	if (Inventory->GetEquippedWeaponInstance())
	{
		SpawnWeaponToSheath();
	}
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
	
	// 인벤토리의 현재 무기 슬롯 사용
	UItemInstance* WeaponItem = Inventory->GetEquippedWeaponInstance();

	if (!WeaponItem)
	{
		return;
	}
	
	// 이미 장착 중이면 해제
	if (StateComp->HasStateTag(CombatTags::State_Combat_Armed))
	{
		Unequip();
	}
	else
	{
		Equip(WeaponItem);
	}
}

bool UPlayerEquipmentComponent::Equip(UItemInstance* Item)
{
	if (!OwnerCharacter || !Item || PendingEquipItem)
	{
		return false;
	}

	if (!CurrentWeaponData || !CurrentWeaponData->EquipMontage)
	{
		return false;
	}
	
	PendingEquipItem = Item;
	
	StateComp->AddStateTag(CombatTags::State_Action_Equipping);
	
	UAnimInstance* AnimInstance =
		OwnerCharacter->GetMesh()->GetAnimInstance();
	
	if (!AnimInstance)
	{
		PendingEquipItem = nullptr;
		StateComp->RemoveStateTag(CombatTags::State_Action_Equipping);
		return false;
	}
	
	const float Duration =
		AnimInstance->Montage_Play(CurrentWeaponData->EquipMontage);

	if (Duration <= 0.f)
	{
		PendingEquipItem = nullptr;
		StateComp->RemoveStateTag(CombatTags::State_Action_Equipping);
		return false;
	}
	
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(
		this,
		&UPlayerEquipmentComponent::OnEquipMontageEnded);
	
	AnimInstance->Montage_SetEndDelegate(
		EndDelegate,
		CurrentWeaponData->EquipMontage);
	
	return true;
}

void UPlayerEquipmentComponent::AnimNotify_Equip()
{
	if (!PendingEquipItem)
	{
		return;
	}

	if (!CurrentWeaponData)
	{
		return;
	}
	
	if (!EquipWeapon(PendingEquipItem, CurrentWeaponData))
	{
		PendingEquipItem = nullptr;
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

	if (!EquippedWeapon)
	{
		if (!WeaponData || !WeaponData->WeaponClass)
		{
			return false;
		}
		
		FActorSpawnParameters Params;
		Params.Owner = OwnerCharacter;
		Params.Instigator = OwnerCharacter;

		EquippedWeapon =
			GetWorld()->SpawnActor<AWeaponBase>(
				WeaponData->WeaponClass,
				Params);

		if (!EquippedWeapon)
		{
			return false;
		}
	}

	EquippedWeapon->AttachToComponent(
	OwnerCharacter->GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		WeaponData->EquipSocket);

	CurrentWeaponInstance = Item;

	StateComp->AddStateTag(CombatTags::State_Combat_Armed);
	
	return true;
}

void UPlayerEquipmentComponent::Unequip()
{
	if (!CurrentWeaponInstance)
	{
		return;
	}
	
	if (!CurrentWeaponData || !CurrentWeaponData->UnequipMontage)
	{
		return;
	}

	StateComp->AddStateTag(CombatTags::State_Action_Equipping);
	
	UAnimInstance* AnimInstance =
		OwnerCharacter->GetMesh()->GetAnimInstance();
	
	if (!AnimInstance)
	{
		StateComp->RemoveStateTag(CombatTags::State_Action_Equipping);
		return;
	}
	
	const float Duration =
		AnimInstance->Montage_Play(CurrentWeaponData->UnequipMontage);
	
	if (Duration <= 0.f)
	{
		StateComp->RemoveStateTag(CombatTags::State_Action_Equipping);
		return;
	}
	
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(
		this,
		&UPlayerEquipmentComponent::OnUnequipMontageEnded);

	AnimInstance->Montage_SetEndDelegate(
		EndDelegate,
		CurrentWeaponData->UnequipMontage);
}

void UPlayerEquipmentComponent::AnimNotify_Unequip()
{
	UnequipWeapon();
}

void UPlayerEquipmentComponent::UnequipWeapon()
{
	if (!EquippedWeapon)
	{
		return;
	}
	
	if (!CurrentWeaponData)
	{
		return;
	}
	
	EquippedWeapon->AttachToComponent(
		OwnerCharacter->GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		CurrentWeaponData->UnequipSocket);

	CurrentWeaponInstance = nullptr;

	if (StateComp->HasStateTag(CombatTags::State_Combat_Armed))
	{
		StateComp->RemoveStateTag(CombatTags::State_Combat_Armed);
	}
}

void UPlayerEquipmentComponent::OnEquipMontageEnded(
	UAnimMontage* Montage,
	bool bInterrupted)
{
	PendingEquipItem = nullptr;

	if (StateComp->HasStateTag(CombatTags::State_Action_Equipping))
	{
		StateComp->RemoveStateTag(
			CombatTags::State_Action_Equipping);
	}

	if (bInterrupted)
	{
		UE_LOG(LogTemp, Warning, TEXT("Equip Interrupted"));
	}
}

void UPlayerEquipmentComponent::OnUnequipMontageEnded(
	UAnimMontage* Montage,
	bool bInterrupted)
{
	if (StateComp->HasStateTag(CombatTags::State_Action_Equipping))
	{
		StateComp->RemoveStateTag(
			CombatTags::State_Action_Equipping);
	}

	if (bInterrupted)
	{
		UE_LOG(LogTemp, Warning, TEXT("Unequip Interrupted"));
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

	if (StateComp->HasStateTag(CombatTags::State_Action_UsingItem))
	{
		return false;
	}
	
	if (StateComp->HasStateTag(CombatTags::State_Hit_Dead))
	{
		return false;
	}

	if (StateComp->HasStateTag(CombatTags::State_Action_Executing))
	{
		return false;
	}
	
	return true;
}

void UPlayerEquipmentComponent::SpawnWeaponToSheath()
{
	UItemInstance* Item = Inventory->GetEquippedWeaponInstance();

	if (!Item)
	{
		return;
	}
	
	CurrentWeaponInstance = Item;
	CurrentWeaponData = 
		Cast<UWeaponDataAsset>(Item->GetItemData());
	
	if (!CurrentWeaponData)
	{
		return;
	}
	
	FActorSpawnParameters Params;
	Params.Owner = OwnerCharacter;

	EquippedWeapon =
		GetWorld()->SpawnActor<AWeaponBase>(
			CurrentWeaponData->WeaponClass,
			Params);

	EquippedWeapon->AttachToComponent(
		OwnerCharacter->GetMesh(),
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		CurrentWeaponData->UnequipSocket);
}