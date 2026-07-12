
#include "Animation/Notifies/AN_DeadFinal.h"

#include "Components/SkeletalMeshComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerDefenseComponent.h"

void UAN_DeadFinal::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
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

	DefenseComponent->FinalizeDead();
}
