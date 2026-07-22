#include "Entity/Player/PlayerItemUseComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerInventoryComponent.h"
#include "Entity/Player/PlayerLocomotionComponent.h"
#include "Framework/DataAsset/ItemDataAsset.h"
#include "Framework/DataAsset/ConsumableDataAsset.h"
#include "Entity/Item/ItemEffect.h"
#include "Entity/Item/ItemInstance.h"
#include "InputActionValue.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameplayTags/CombatGameplayTags.h"
#include "Entity/Player/StateTagComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Framework/Subsystem/KatanaSoundManagerSubsystem.h"

UPlayerItemUseComponent::UPlayerItemUseComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
}

void UPlayerItemUseComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<APlayerCharacterBase>(GetOwner());

	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerItemUseComponent : OwnerCharacter is nullptr"));
		return;
	}

	Inventory = OwnerCharacter->GetInventoryComponent();

	if (!Inventory)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerItemUseComponent : Inventory is nullptr"));
		return;
	}

	StateComp = OwnerCharacter->GetStateTagComponent();

	if (!StateComp)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerItemUseComponent : StateComponent is nullptr"));
		return;
	}

	Locomotion = OwnerCharacter->GetLocomotionComponent();

	if (!Locomotion)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerItemUseComponent : LocomotionComponent is nullptr"));
		return;
	}
}

void UPlayerItemUseComponent::UseConsumableInput(
	const FInputActionValue& Value)
{
	if (!Inventory)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerItemUseComponent : Inventory is nullptr"));
		return;
	}

	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerItemUseComponent : OwnerCharacter is nullptr"));
		return;
	}

	if (!Inventory->GetCurrentConsumable())
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerItemUseComponent : CurrentConsumable is nullptr"));
		return;
	}

	if (!Inventory->GetCurrentConsumable()->GetItemData())
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerItemUseComponent : ConsumableData is nullptr"));
		return;
	}

	if (!canUseItem())
	{
		return;
	}

	CurrentUsingItem = Inventory->GetCurrentConsumable();

	StateComp->AddStateTag(CombatTags::State_Action_UsingItem);

	Locomotion->RefreshMovementSettings();

	UAnimInstance* AnimInstance =
		OwnerCharacter->GetMesh()->GetAnimInstance();

	if (!AnimInstance)
	{
		StateComp->RemoveStateTag(CombatTags::State_Action_UsingItem);
		return;
	}

	const float Duration =
		AnimInstance->Montage_Play(
			Inventory->GetCurrentConsumable()->GetItemData()->UseMontage,
			Inventory->GetCurrentConsumable()->GetItemData()->PlayRate);

	if (Duration <= 0.f)
	{
		StateComp->RemoveStateTag(CombatTags::State_Action_UsingItem);
		return;
	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(
		this,
		&UPlayerItemUseComponent::OnItemUseMontageEnded);

	AnimInstance->Montage_SetEndDelegate(
		EndDelegate,
		Inventory->GetCurrentConsumable()->GetItemData()->UseMontage);
}

void UPlayerItemUseComponent::AnimNotify_UseConsumable()
{
	if (!Inventory)
	{
		return;
	}

	UseItem(Inventory->GetCurrentConsumable());
}

float UPlayerItemUseComponent::GetCurrentMoveSpeedMultiplier() const
{
	if (!CurrentUsingItem)
	{
		return 1.0f;
	}

	const UItemDataAsset* Data = CurrentUsingItem->GetItemData();

	return Data ? Data->MoveSpeedMultiplier : 1.0f;
}

void UPlayerItemUseComponent::UseItem(UItemInstance* Item)
{
	if (!Item)
	{
		return;
	}

	if (const UConsumableDataAsset* Consumable =
		Cast<UConsumableDataAsset>(Item->GetItemData()))
	{
		UseConsumable(Item, Consumable);
	}
}

void UPlayerItemUseComponent::UseConsumable(
	UItemInstance* Item,
	const UConsumableDataAsset* ConsumableData)
{
	if (!Item || !ConsumableData)
	{
		return;
	}

	if(!ApplyConsumableEffects(ConsumableData))
	{
		return;
	}

	UKatanaSoundManagerSubsystem::PlaySoundAtLocation(
		this,
		EAudioType::SFX,
		CurrentUsingItem->GetItemData()->UseSound,
		OwnerCharacter->GetActorLocation());

	UE_LOG(LogTemp, Warning, TEXT("Item Use %s, Item Count : %d"),
			*ConsumableData->ItemName.ToString(),
			Item->GetCount());

	if (ConsumableData->bConsumeOnUse)
	{
		Inventory->RemoveItem(Item, 1);
	}
}

bool UPlayerItemUseComponent::ApplyConsumableEffects(
	const UConsumableDataAsset* ConsumableData)
{
	if (!ConsumableData)
	{
		return false;
	}

	AActor* OwnerActor = GetOwner();

	for (UItemEffect* Effect : ConsumableData->Effects)
	{
		if (!Effect)
		{
			continue;
		}

		if (!Effect->CanApply(OwnerActor))
		{
			return false;
		}
	}

	for (UItemEffect* Effect : ConsumableData->Effects)
	{
		if (!Effect)
		{
			continue;
		}

		Effect->Apply(OwnerActor);
	}

	return true;
}

void UPlayerItemUseComponent::OnItemUseMontageEnded(
	UAnimMontage* Montage,
	bool bInterrupted)
{
	StateComp->RemoveStateTag(CombatTags::State_Action_UsingItem);
	Locomotion->RefreshMovementSettings();

	HideItemMesh();
}

bool UPlayerItemUseComponent::canUseItem()
{
	if (!StateComp)
	{
		return false;
	}

	if (StateComp->HasStateTag(CombatTags::State_Action_UsingItem))
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

	if (StateComp->HasStateTag(CombatTags::State_Action_Executing))
	{
		return false;
	}

	return true;
}

void UPlayerItemUseComponent::AnimNotify_ShowItem(bool InShowItem, FName SocketName)
{
	if (!ItemMesh)
	{
		return;
	}

	if (!CurrentUsingItem)
	{
		return;
	}

	if (InShowItem)
	{
		ShowItemMesh(SocketName);
	}
	else
	{
		HideItemMesh();
	}
}

void UPlayerItemUseComponent::ShowItemMesh(FName InSocketName) const
{
	const UItemDataAsset* ItemData = CurrentUsingItem->GetItemData();
	if (!ItemData)
	{
		return;
	}

	UStaticMesh* ItemStaticMesh = ItemData->StaticMesh;
	if (ItemStaticMesh
		&& ItemMesh
		)
	{
		ItemMesh->SetStaticMesh(ItemStaticMesh);
		ItemMesh->SetHiddenInGame(false);
		ItemMesh->AttachToComponent(OwnerCharacter->GetMesh(),
			FAttachmentTransformRules::SnapToTargetIncludingScale,
			InSocketName);
	}
}

void UPlayerItemUseComponent::HideItemMesh() const
{
	if (ItemMesh)
	{
		ItemMesh->SetStaticMesh(nullptr);
		ItemMesh->SetHiddenInGame(true);
	}
}
