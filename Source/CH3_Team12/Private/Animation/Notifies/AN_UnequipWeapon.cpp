#include "Animation/Notifies/AN_UnequipWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerEquipmentComponent.h"

void UAN_UnequipWeapon::Notify(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp)
	{
		return;
	}

	APlayerCharacterBase* Character =
		Cast<APlayerCharacterBase>(MeshComp->GetOwner());

	if (!Character)
	{
		return;
	}

	if (UPlayerEquipmentComponent* Equipment =
		Character->FindComponentByClass<UPlayerEquipmentComponent>())
	{
		Equipment->AnimNotify_Unequip();
	}
}