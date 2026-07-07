// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NotifyStates/ANS_ParryWindow.h"

#include "Components/SkeletalMeshComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerCombatComponent.h"

void UANS_ParryWindow::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyBegin(
		MeshComp,
		Animation,
		TotalDuration,
		EventReference
	);

	if (!MeshComp)
	{
		return;
	}

	APlayerCharacterBase* PlayerCharacter =
		Cast<APlayerCharacterBase>(MeshComp->GetOwner());

	if (!PlayerCharacter)
	{
		return;
	}

	if (UPlayerCombatComponent* CombatComponent =
		PlayerCharacter->GetCombatComponent())
	{
		CombatComponent->OpenParryWindow();
	}
}

void UANS_ParryWindow::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyEnd(
		MeshComp,
		Animation,
		EventReference
	);

	if (!MeshComp)
	{
		return;
	}

	APlayerCharacterBase* PlayerCharacter =
		Cast<APlayerCharacterBase>(MeshComp->GetOwner());

	if (!PlayerCharacter)
	{
		return;
	}

	if (UPlayerCombatComponent* CombatComponent =
		PlayerCharacter->GetCombatComponent())
	{
		CombatComponent->CloseParryWindow();
	}
}