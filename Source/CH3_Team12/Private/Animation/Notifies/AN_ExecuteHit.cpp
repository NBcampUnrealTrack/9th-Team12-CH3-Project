#include "Animation/Notifies/AN_ExecuteHit.h"
#include "Components/SkeletalMeshComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerAttackComponent.h"

void UAN_ExecuteHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
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
	
	if (UPlayerAttackComponent* AttackComponent = Character->GetAttackComponent())
	{
		AttackComponent->ExecutionHitNotify();
	}
}
