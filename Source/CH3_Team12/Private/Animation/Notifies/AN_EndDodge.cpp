#include "Animation/Notifies/AN_EndDodge.h"

#include "Components/SkeletalMeshComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerLocomotionComponent.h"

void UAN_EndDodge::Notify(
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

	UPlayerLocomotionComponent* LocomotionComponent =
		PlayerCharacter->GetLocomotionComponent();

	if (!LocomotionComponent)
	{
		return;
	}

	LocomotionComponent->EndDodge();
}