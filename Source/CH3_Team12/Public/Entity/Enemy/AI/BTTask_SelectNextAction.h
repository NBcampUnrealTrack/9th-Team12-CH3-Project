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
	int32 MoveWeight = 30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	int32 NormalAttackWeight = 50;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	int32 StrongAttackWeight = 20;
};
