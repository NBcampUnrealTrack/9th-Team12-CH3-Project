// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/AI/EnemyAIController.h"

#include "TimerManager.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/Engine.h"
#include "Entity/Enemy/EnemyCharacterBase.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogEnemyAIController, Log, All);

AEnemyAIController::AEnemyAIController()
{
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackBoardComponent"));
}


void AEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("EnemyAIController OnPossess"));
	}

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

	UpdateTargetActor();

	GetWorldTimerManager().SetTimer(
		TargetActorTimerHandle,
		this,
		&AEnemyAIController::UpdateTargetActor,
		0.2f,
		true
	);

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


void AEnemyAIController::UpdateTargetActor()
{
	if (!BlackboardComponent)
	{
		UE_LOG(LogEnemyAIController, Error, TEXT("UpdateTargetActor Failed: BlackboardComponent is null"));
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn)
	{
		UE_LOG(LogEnemyAIController, Warning, TEXT("UpdateTargetActor Failed: PlayerPawn is null"));
	}

	BlackboardComponent->SetValueAsObject(TargetActorKeyName, PlayerPawn);

	UE_LOG(LogEnemyAIController, Log, TEXT("UpdateTargetActor Success: %s"), *GetNameSafe(PlayerPawn));

	GetWorldTimerManager().ClearTimer(TargetActorTimerHandle);
}
