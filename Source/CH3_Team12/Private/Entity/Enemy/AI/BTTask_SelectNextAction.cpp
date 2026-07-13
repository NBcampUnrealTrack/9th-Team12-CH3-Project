// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/AI/BTTask_SelectNextAction.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SelectNextAction::UBTTask_SelectNextAction()
{
	NodeName = TEXT("Select Next Action");
}

EBTNodeResult::Type UBTTask_SelectNextAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	if (!BlackboardComponent->GetValueAsBool(CanStartCombatKeyName))
	{
		return EBTNodeResult::Failed;
	}

	float DistanceToTarget = BlackboardComponent->GetValueAsFloat(DistanceToTargetKeyName);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorKeyName));
	if (ControlledPawn && TargetActor)
	{
		DistanceToTarget = FVector::Dist(ControlledPawn->GetActorLocation(), TargetActor->GetActorLocation());
		BlackboardComponent->SetValueAsFloat(DistanceToTargetKeyName, DistanceToTarget);
	}

	int32 SelectedAction = 1;
	int32 SelectedPattern = 0;
	const bool bOpeningAttackChecked = BlackboardComponent->GetValueAsBool(OpeningAttackCheckedKeyName);
	BlackboardComponent->SetValueAsBool(OpeningAttackSelectedKeyName, false);

	bool bUseOpeningAttack = false;
	if (!bOpeningAttackChecked)
	{
		if (DistanceToTarget >= FarSpecialAttackDistance)
		{
			SelectedAction = 2;
			SelectedPattern = 1;
			BlackboardComponent->SetValueAsBool(OpeningAttackSelectedKeyName, true);
			bUseOpeningAttack = true;
		}
	}

	if (!bUseOpeningAttack)
	{
		const int32 TotalWeight = NormalAttack0Weight + NormalAttack1Weight + StrongAttack0Weight;
		if (TotalWeight <= 0)
		{
			return EBTNodeResult::Failed;
		}

		const int32 RandomValue = FMath::RandRange(1, TotalWeight);

		if (RandomValue <= NormalAttack0Weight)
		{
			SelectedAction = 1;
			SelectedPattern = 0;
		}
		else if (RandomValue <= NormalAttack0Weight + NormalAttack1Weight)
		{
			SelectedAction = 1;
			SelectedPattern = 1;
		}
		else
		{
			SelectedAction = 2;
			SelectedPattern = 0;
		}
	}


	BlackboardComponent->SetValueAsInt(SelectedActionKeyName, SelectedAction);
	BlackboardComponent->SetValueAsInt(SelectedPatternKeyName, SelectedPattern);

	return EBTNodeResult::Succeeded;
}
