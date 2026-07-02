// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/AI/EnemyAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Entity/Enemy/EnemyCharacterBase.h"
#include "Kismet/GameplayStatics.h"

AEnemyAIController::AEnemyAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComponent"));
}


void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!BehaviorTreeComponent || !BlackboardComponent)
	{
		return;
	}

	AEnemyCharacterBase* EnemyCharacter = Cast<AEnemyCharacterBase>(InPawn);
	if (!EnemyCharacter)
	{
		return;
	}

	UBehaviorTree* EnemyBehaviorTreeAsset = EnemyCharacter->GetBehaviorTreeAsset();
	if (!EnemyBehaviorTreeAsset)
	{
		return;
	}


	UBlackboardData* BlackboardAsset = EnemyBehaviorTreeAsset->BlackboardAsset;
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


	BlackboardComponent->SetValueAsVector(HomeLocationKeyName, InPawn->GetActorLocation());

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (PlayerPawn)
	{
		BlackboardComponent->SetValueAsObject(TargetActorKeyName, PlayerPawn);
	}

	BehaviorTreeComponent->StartTree(*EnemyBehaviorTreeAsset);
}

void AEnemyAIController::OnUnPossess()
{
	if (BehaviorTreeComponent)
	{
		BehaviorTreeComponent->StopTree(EBTStopMode::Safe);
	}

	Super::OnUnPossess();
}
