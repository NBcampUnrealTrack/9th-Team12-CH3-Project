// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/AI/BTTask_SelectStrafeLocation.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"


UBTTask_SelectStrafeLocation::UBTTask_SelectStrafeLocation()
{
	NodeName = TEXT("Select Strafe Location");
}

EBTNodeResult::Type UBTTask_SelectStrafeLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	const FVector EnemyLocation = ControlledPawn->GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();

	FVector DirectionFromTarget = EnemyLocation - TargetLocation;
	DirectionFromTarget.Z = 0.f;

	if (DirectionFromTarget.IsNearlyZero())
	{
		DirectionFromTarget = -TargetActor->GetActorForwardVector();
		DirectionFromTarget.Z = 0.f;
	}

	DirectionFromTarget.Normalize();

	const FVector RightVector = FVector::CrossProduct(FVector::UpVector, DirectionFromTarget).GetSafeNormal();
	const float SideSign = FMath::RandBool() ? 1.f : -1.f;

	const FVector StrafeLocation =
		TargetLocation +
		DirectionFromTarget * DesiredDistance +
		RightVector * StrafeOffset * SideSign;

	BlackboardComponent->SetValueAsVector(StrafeLocationKeyName, StrafeLocation);

	return EBTNodeResult::Succeeded;
}
