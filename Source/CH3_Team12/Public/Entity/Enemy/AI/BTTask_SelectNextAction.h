// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SelectNextAction.generated.h"

/**
 * 
 */
UCLASS()
class CH3_TEAM12_API UBTTask_SelectNextAction : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SelectNextAction();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	FName SelectedActionKeyName = TEXT("SelectedAction");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	FName SelectedPatternKeyName = TEXT("SelectedPattern");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	FName DistanceToTargetKeyName = TEXT("DistanceToTarget");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	FName TargetActorKeyName = TEXT("TargetActor");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float FarSpecialAttackDistance = 2400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	int32 NormalAttack0Weight = 40;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	int32 NormalAttack1Weight = 40;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	int32 StrongAttack0Weight = 20;
};
