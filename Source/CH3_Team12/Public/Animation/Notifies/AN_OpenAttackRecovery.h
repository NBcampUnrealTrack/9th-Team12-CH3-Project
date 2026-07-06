// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_OpenAttackRecovery.generated.h"

/**
 * 
 */
UCLASS(meta=(DisplayName="Open Attack Recovery"))
class CH3_TEAM12_API UAN_OpenAttackRecovery : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;
};

