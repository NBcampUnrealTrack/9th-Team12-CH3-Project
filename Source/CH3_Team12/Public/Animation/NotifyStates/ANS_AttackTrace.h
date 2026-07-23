// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_AttackTrace.generated.h"

struct FHitBoxData;

/**
 * 
 */
UCLASS(meta=(DisplayName="Attack Trace"))
class CH3_TEAM12_API UANS_AttackTrace : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float TotalDuration,
		const FAnimNotifyEventReference& EventReference
	) override;

	virtual void NotifyTick(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		float FrameDeltaTime,
		const FAnimNotifyEventReference& EventReference
	) override;

	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;
	
public:
	UPROPERTY(EditAnywhere)
	int32 HitIndex = 0;
	
	UPROPERTY(EditAnywhere, Category = "Animation | HitBox")
	TArray<FHitBoxData> HitBoxDatas;
	
};