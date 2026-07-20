// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notifies/AN_ShowItemMesh.h"
#include "Components/SkeletalMeshComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerItemUseComponent.h"


void UAN_ShowItemMesh::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
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

	if (UPlayerItemUseComponent* ItemUseComponent = Character->GetItemUseComponent())
	{
		ItemUseComponent->AnimNotify_ShowItem(bShowItem, SocketName);
	}
}
