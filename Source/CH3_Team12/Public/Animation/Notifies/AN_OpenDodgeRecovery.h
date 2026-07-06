#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_OpenDodgeRecovery.generated.h"

UCLASS(meta=(DisplayName="Open Dodge Recovery"))
class CH3_TEAM12_API UAN_OpenDodgeRecovery : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;
};