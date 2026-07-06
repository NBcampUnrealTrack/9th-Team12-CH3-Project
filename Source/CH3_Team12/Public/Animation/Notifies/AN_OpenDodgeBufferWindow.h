#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_OpenDodgeBufferWindow.generated.h"

UCLASS(meta=(DisplayName="Open Dodge Buffer Window"))
class CH3_TEAM12_API UAN_OpenDodgeBufferWindow : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;
};