#include "Animation/NotifyStates/ANS_DodgeInvincible.h"

#include "Components/SkeletalMeshComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerDefenseComponent.h"

void UANS_DodgeInvincible::NotifyBegin(
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

	UPlayerDefenseComponent* DefenseComponent =
		PlayerCharacter->GetDefenseComponent();

	if (!DefenseComponent)
	{
		return;
	}

	DefenseComponent->EnableInvincible();
}

void UANS_DodgeInvincible::NotifyEnd(
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

	UPlayerDefenseComponent* DefenseComponent =
		PlayerCharacter->GetDefenseComponent();

	if (!DefenseComponent)
	{
		return;
	}

	DefenseComponent->DisableInvincible();
}