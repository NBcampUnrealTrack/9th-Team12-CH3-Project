// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notifies/AN_Footstep.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/FootstepComponent.h"

void UAN_Footstep::Notify(
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

	UFootstepComponent* FootstepComponent =
		Character->GetFootstepComponent();

	if (!FootstepComponent)
	{
		return;
	}

	const FName SocketName =
		FootSide == EFootstepSide::Left
			? LeftFootSocketName
			: RightFootSocketName;

	FootstepComponent->PlayFootstep(SocketName);
}
