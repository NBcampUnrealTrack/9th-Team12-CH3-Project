#include "Animation/NotifyStates//ANS_DashAttackWindow.h"

#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequenceBase.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerAttackComponent.h"

void UANS_DashAttackWindow::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
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

	APlayerCharacterBase* Player =
		Cast<APlayerCharacterBase>(MeshComp->GetOwner());

	if (!Player)
	{
		return;
	}

	UPlayerAttackComponent* AttackComponent =
		Player->GetAttackComponent();

	if (!AttackComponent)
	{
		return;
	}

	AttackComponent->OpenDashAttackWindow();
}

void UANS_DashAttackWindow::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
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

	APlayerCharacterBase* Player =
		Cast<APlayerCharacterBase>(MeshComp->GetOwner());

	if (!Player)
	{
		return;
	}

	UPlayerAttackComponent* AttackComponent =
		Player->GetAttackComponent();

	if (!AttackComponent)
	{
		return;
	}

	AttackComponent->CloseDashAttackWindow();
}