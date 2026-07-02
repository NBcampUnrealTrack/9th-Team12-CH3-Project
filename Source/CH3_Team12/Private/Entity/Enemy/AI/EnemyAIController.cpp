// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/AI/EnemyAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AEnemyAIController::AEnemyAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComponent"));
}


void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!BehaviorTreeComponent)
	{
		return;
	}


	UBlackboardData* BlackboardAsset = BehaviorTreeAsset->BlackboardAsset;
	if (!BlackboardAsset)
	{
		return;
	}

	// NOTE: UseBlackboard() = UBlackboardComponent*& 요구 
	// NOTE: TObjectPtr을 바로 넘기면 컴파일 에러
	UBlackboardComponent* RawBlackboardComponent = BlackboardComponent.Get();
	if (!UseBlackboard(BlackboardAsset, RawBlackboardComponent))
	{
		return;
	}

	if (InPawn)
	{
		BlackboardComponent->SetValueAsVector(HomeLocationKeyName, InPawn->GetActorLocation());
	}

	BehaviorTreeComponent->StartTree(*BehaviorTreeAsset);
}

void AEnemyAIController::OnUnPossess()
{
	if (BehaviorTreeComponent)
	{
		BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
	}

	Super::OnUnPossess();
}
