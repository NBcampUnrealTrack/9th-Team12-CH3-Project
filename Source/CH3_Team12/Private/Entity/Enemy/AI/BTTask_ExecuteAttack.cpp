// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/AI/BTTask_ExecuteAttack.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Entity/Enemy/EnemyCharacterBase.h"
#include "Entity/Enemy/Component/EnemyAttackComponent.h"

UBTTask_ExecuteAttack::UBTTask_ExecuteAttack()
{
	NodeName = TEXT("Execute Attack");
	bCreateNodeInstance = true;
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

	const int32 SelectedAction = BlackboardComponent->GetValueAsInt(SelectedActionKeyName);
	const int32 SelectedPattern = BlackboardComponent->GetValueAsInt(SelectedPatternKeyName);

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

	CachedOwnerComp = &OwnerComp;
	CachedEnemyAttackComponent = EnemyAttackComponent;
	EnemyAttackComponent->OnAttackFinished.AddDynamic(this, &UBTTask_ExecuteAttack::HandleAttackFinished);
	const bool bAttackStarted = EnemyAttackComponent->ExecuteAttack(TargetActor, SelectedAction, SelectedPattern);

	if (!bAttackStarted)
	{
		EnemyAttackComponent->OnAttackFinished.RemoveDynamic(
			this,
			&UBTTask_ExecuteAttack::HandleAttackFinished
		);

		CachedOwnerComp = nullptr;
		CachedEnemyAttackComponent = nullptr;

		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}


void UBTTask_ExecuteAttack::HandleAttackFinished()
{
	if (!CachedOwnerComp)
	{
		return;
	}

	return FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
}

void UBTTask_ExecuteAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory
                                           , EBTNodeResult::Type TaskResult)
{
	if (CachedEnemyAttackComponent)
	{
		CachedEnemyAttackComponent->OnAttackFinished.RemoveDynamic(
			this,
			&UBTTask_ExecuteAttack::HandleAttackFinished
		);
	}

	CachedOwnerComp = nullptr;
	CachedEnemyAttackComponent = nullptr;

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
