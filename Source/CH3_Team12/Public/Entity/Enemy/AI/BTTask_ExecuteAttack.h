// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ExecuteAttack.generated.h"

class UEnemyAttackComponent;
/**
 * 
 */
UCLASS()
class CH3_TEAM12_API UBTTask_ExecuteAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ExecuteAttack();

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory
	                            , EBTNodeResult::Type TaskResult) override;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	FName TargetActorKeyName = TEXT("TargetActor");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	FName SelectedActionKeyName = TEXT("SelectedAction");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	FName SelectedPatternKeyName = TEXT("SelectedPattern");

private:
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;

	UPROPERTY()
	TObjectPtr<UEnemyAttackComponent> CachedEnemyAttackComponent;

	UFUNCTION()
	void HandleAttackFinished();
};
