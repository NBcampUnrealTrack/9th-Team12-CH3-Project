#include "Animation/NotifyStates/ANS_AttackTrace.h"
#include "Components/SkeletalMeshComponent.h"
#include "Interface/AnimationAttackInterface.h"
#include "Framework/HitBoxData.h"

void UANS_AttackTrace::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp)
	{
		return;
	}
	
	if (IAnimationAttackInterface* AttackPawn = Cast<IAnimationAttackInterface>(MeshComp->GetOwner()))
	{
		AttackPawn->AttackHitCheckStart(HitIndex, HitBoxDatas);
	}
}

void UANS_AttackTrace::NotifyTick(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float FrameDeltaTime,
	const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp)
	{
		return;
	}

	if (IAnimationAttackInterface* AttackPawn = Cast<IAnimationAttackInterface>(MeshComp->GetOwner()))
	{
		AttackPawn->AttackHitCheckTick();
	}
}

void UANS_AttackTrace::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	if (IAnimationAttackInterface* AttackPawn = Cast<IAnimationAttackInterface>(MeshComp->GetOwner()))
	{
		AttackPawn->AttackHitCheckEnd();
	}
}