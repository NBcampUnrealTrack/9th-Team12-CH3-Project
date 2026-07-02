// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/AI/BTTask_SelectNextAction.h"

#include "BehaviorTree/BlackboardComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBTTaskSelectNextAction, Log, All);

UBTTask_SelectNextAction::UBTTask_SelectNextAction()
{
	NodeName = TEXT("Select Next Action");
}

EBTNodeResult::Type UBTTask_SelectNextAction::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		UE_LOG(LogBTTaskSelectNextAction, Error, TEXT("Failed: BlackboardComponent is null"));
		return EBTNodeResult::Failed;
	}

	const int32 TotalWeight = MoveWeight + NormalAttackWeight + StrongAttackWeight;
	if (TotalWeight <= 0)
	{
		UE_LOG(LogBTTaskSelectNextAction, Error, TEXT("Failed: TotalWeight is invalid"));
		return EBTNodeResult::Failed;
	}

	const int32 RandomValue = FMath::RandRange(1, TotalWeight);

	int32 SelectedAction = 0;

	if (RandomValue <= MoveWeight)
	{
		SelectedAction = 0;
	}
	else if (RandomValue <= MoveWeight + NormalAttackWeight)
	{
		SelectedAction = 1;
	}
	else
	{
		SelectedAction = 2;
	}

	const int32 SelectedPattern = FMath::RandRange(0, 1);

	BlackboardComponent->SetValueAsInt(SelectedActionKeyName, SelectedAction);
	BlackboardComponent->SetValueAsInt(SelectedPatternKeyName, SelectedPattern);

	UE_LOG(
		LogBTTaskSelectNextAction,
		Log,
		TEXT("SelectedAction: %d, SelectedPattern: %d"),
		SelectedAction,
		SelectedPattern
	);

	return EBTNodeResult::Succeeded;
}
