// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_ShowItemMesh.generated.h"

/**
 * 
 */
UCLASS()
class CH3_TEAM12_API UAN_ShowItemMesh : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp,
		UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	uint8 bShowItem:1 = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item")
	FName SocketName = NAME_None;
};
