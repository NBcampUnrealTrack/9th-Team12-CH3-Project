#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_Footstep.generated.h"

UENUM(BlueprintType)
enum class EFootstepSide : uint8
{
	Left,
	Right
};

UCLASS()
class CH3_TEAM12_API UAN_Footstep : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Footstep")
	EFootstepSide FootSide = EFootstepSide::Left;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Footstep")
	FName LeftFootSocketName = TEXT("foot_l");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Footstep")
	FName RightFootSocketName = TEXT("foot_r");

	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;
};