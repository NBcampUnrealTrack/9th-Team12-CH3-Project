#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_CanDodgeAttack.generated.h"

UCLASS()
class CH3_TEAM12_API UAN_CanDodgeAttack : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;
};
