#include "Animation/Notifies/AN_OpenComboWindow.h"

#include "Components/SkeletalMeshComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerAttackComponent.h"

void UAN_OpenComboWindow::Notify(
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

	CombatComponent->OpenComboWindow();
}