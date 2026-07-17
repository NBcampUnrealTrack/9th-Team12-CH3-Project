#include "Animation/Notifies/AN_CanDodgeAttack.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerAttackComponent.h"
#include "Components/SkeletalMeshComponent.h"

void UAN_CanDodgeAttack::Notify(
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
	
	if (UPlayerAttackComponent* AttackComponent = Character->GetAttackComponent())
	{
		AttackComponent->OpenDashAttackWindow();
	}
}