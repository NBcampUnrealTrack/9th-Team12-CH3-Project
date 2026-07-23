#include "Animation/Notifies/AN_EquipWeapon.h"
#include "Components/SkeletalMeshComponent.h"
#include "Entity/Player/PlayerEquipmentComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"

void UAN_EquipWeapon::Notify(
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

	if (UPlayerEquipmentComponent* Equipment = Character->GetEquipmentComponent())
	{
		Equipment->AnimNotify_Equip();
	}
}