// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RotateToTarget.generated.h"

/**
 * 
 */
UCLASS()
class CH3_TEAM12_API UBTTask_RotateToTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_RotateToTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	FName TargetActorKeyName = TEXT("TargetActor");
};
