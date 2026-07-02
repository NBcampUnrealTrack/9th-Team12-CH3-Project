// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/AI/BTTask_ExecuteAttack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Entity/Enemy/EnemyCharacterBase.h"
#include "Entity/Enemy/Component/EnemyAttackComponent.h"

UBTTask_ExecuteAttack::UBTTask_ExecuteAttack()
{
	NodeName = TEXT("Execute Attack");
}

EBTNodeResult::Type UBTTask_ExecuteAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorKeyName));
	if (!TargetActor)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	AEnemyCharacterBase* EnemyCharacter = Cast<AEnemyCharacterBase>(AIController->GetPawn());
	if (!EnemyCharacter)
	{
		return EBTNodeResult::Failed;
	}

	UEnemyAttackComponent* EnemyAttackComponent = EnemyCharacter->GetEnemyAttackComponent();
	if (!EnemyAttackComponent)
	{
		return EBTNodeResult::Failed;
	}

	const bool bAttackStarted = EnemyAttackComponent->ExecuteAttack(TargetActor);
	return bAttackStarted ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
