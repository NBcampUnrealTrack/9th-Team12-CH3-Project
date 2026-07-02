// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/AI/BTTask_RotateToTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogBTTaskRotateToTarget, Log, All);

UBTTask_RotateToTarget::UBTTask_RotateToTarget()
{
	NodeName = TEXT("Rotate To Target");
}

EBTNodeResult::Type UBTTask_RotateToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UE_LOG(LogBTTaskRotateToTarget, Log, TEXT("ExecuteTask: Rotate To Target"));

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		UE_LOG(LogBTTaskRotateToTarget, Error, TEXT("Failed: BlackboardComponent is null"));
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorKeyName));
	if (!TargetActor)
	{
		UE_LOG(LogBTTaskRotateToTarget, Error, TEXT("Failed: TargetActor is null"));
		return EBTNodeResult::Failed;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		UE_LOG(LogBTTaskRotateToTarget, Error, TEXT("Failed: AIController is null"));
		return EBTNodeResult::Failed;
	}

	APawn* ControlledPawn = AIController->GetPawn();
	if (!ControlledPawn)
	{
		UE_LOG(LogBTTaskRotateToTarget, Error, TEXT("Failed: ControlledPawn is null"));
		return EBTNodeResult::Failed;
	}

	const FVector Direction = TargetActor->GetActorLocation() - ControlledPawn->GetActorLocation();
	const FRotator LookAtRotation = Direction.Rotation();

	ControlledPawn->SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.0f));

	UE_LOG(LogBTTaskRotateToTarget, Log, TEXT("Succeeded: Rotate To Target / Target: %s"), *GetNameSafe(TargetActor));
	return EBTNodeResult::Succeeded;
}
