#include "Animation/Notifies/AN_EndAttack.h"

#include "Components/SkeletalMeshComponent.h"
// #include "Entity/Player/PlayerCharacterBase.h"
// #include "Entity/Player/PlayerCombatComponent.h"
#include "Interface/AnimationAttackInterface.h"

void UAN_EndAttack::Notify(
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
	
	if (IAnimationAttackInterface* AttackPawn = Cast<IAnimationAttackInterface>(MeshComp->GetOwner()))
	{
		AttackPawn->AttackAnimationEnd();
	}
}
