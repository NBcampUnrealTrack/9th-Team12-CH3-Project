// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/AI/BTService_UpdateTargetInfo.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"

UBTService_UpdateTargetInfo::UBTService_UpdateTargetInfo()
{
	NodeName = TEXT("Update Target Info");
	Interval = 0.01f;
	RandomDeviation = 0.f;
}

void UBTService_UpdateTargetInfo::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();

	if (!BlackboardComponent)
	{
		return;
	}

	AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorKeyName));
	if (!TargetActor)
	{
		BlackboardComponent->SetValueAsBool(CanStartCombatKeyName, false);
		return;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		return;
	}

	const FVector PawnLocation = ControlledPawn->GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();

	const float DistanceToTarget = FVector::Dist(PawnLocation, TargetLocation);
	BlackboardComponent->SetValueAsFloat(DistanceToTargetKeyName, DistanceToTarget);

	bool bCanStartCombat = false;
	if (const UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld()))
	{
		FNavLocation ProjectedLocation;
		bCanStartCombat = NavigationSystem->ProjectPointToNavigation(
			TargetLocation,
			ProjectedLocation,
			NavMeshProjectionExtent
		);
	}
	BlackboardComponent->SetValueAsBool(CanStartCombatKeyName, bCanStartCombat);

	const FVector Direction = TargetLocation - PawnLocation;
	const FRotator TargetRotation = Direction.Rotation();
	const FRotator CurrentRotation = ControlledPawn->GetActorRotation();

	const FRotator NewRotation = FMath::RInterpTo(
		CurrentRotation,
		FRotator(0.f, TargetRotation.Yaw, 0.f),
		DeltaSeconds,
		RotationInterpSpeed
	);

	const bool bIsStrafing = BlackboardComponent->GetValueAsBool(TEXT("bIsStrafing"));

	// if (!bIsStrafing)
	// {
	// 	ControlledPawn->SetActorRotation(NewRotation);
	// }
}
