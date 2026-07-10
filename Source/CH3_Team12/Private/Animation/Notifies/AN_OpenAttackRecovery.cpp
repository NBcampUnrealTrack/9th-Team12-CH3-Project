// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/Notifies/AN_OpenAttackRecovery.h"

#include "Components/SkeletalMeshComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerAttackComponent.h"


void UAN_OpenAttackRecovery::Notify(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
)
{
	Super::Notify(
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

	UPlayerAttackComponent* CombatComponent =
		PlayerCharacter->GetCombatComponent();

	if (!CombatComponent)
	{
		return;
	}

	CombatComponent->OpenAttackRecovery();
}