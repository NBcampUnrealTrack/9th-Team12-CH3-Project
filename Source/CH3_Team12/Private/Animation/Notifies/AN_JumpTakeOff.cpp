#include "Animation/Notifies/AN_JumpTakeOff.h"

#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerLocomotionComponent.h"

void UAN_JumpTakeOff::Notify(
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

	APlayerCharacterBase* Player =
		Cast<APlayerCharacterBase>(MeshComp->GetOwner());

	if (!Player)
	{
		return;
	}

	if (UPlayerLocomotionComponent* LocomotionComponent =
		Player->GetLocomotionComponent())
	{
		LocomotionComponent->CommitJump();
	}
}