#include "Animation/Notifies/AN_UseConsumable.h"
#include "Components/SkeletalMeshComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerItemUseComponent.h"

void UAN_UseConsumable::Notify(
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

	if (UPlayerItemUseComponent* ItemUse = Character->GetItemUseComponent())
	{
		ItemUse->AnimNotify_UseConsumable();
	}
}