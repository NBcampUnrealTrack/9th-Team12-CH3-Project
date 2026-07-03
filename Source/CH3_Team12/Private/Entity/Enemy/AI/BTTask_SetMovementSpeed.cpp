// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/AI/BTTask_SetMovementSpeed.h"

#include "AIController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_SetMovementSpeed::UBTTask_SetMovementSpeed()
{
	NodeName = TEXT("Set Movement Speed");
}

EBTNodeResult::Type UBTTask_SetMovementSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
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

	ACharacter* ControlledCharacter = Cast<ACharacter>(ControlledPawn);
	if (!ControlledCharacter)
	{
		return EBTNodeResult::Failed;
	}

	UCharacterMovementComponent* MovementComponent = ControlledCharacter->GetCharacterMovement();
	if (!MovementComponent)
	{
		return EBTNodeResult::Failed;
	}

	MovementComponent->MaxWalkSpeed = MovementSpeed;


	return EBTNodeResult::Succeeded;
}
