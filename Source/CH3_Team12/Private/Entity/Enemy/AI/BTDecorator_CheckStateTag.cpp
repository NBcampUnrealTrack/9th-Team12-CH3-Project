 // Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/AI/BTDecorator_CheckStateTag.h"
#include "Entity/Player/StateTagComponent.h"
#include "AIController.h"

UBTDecorator_CheckStateTag::UBTDecorator_CheckStateTag()
 {
 	NodeName = TEXT("Check State Tags");
 }

bool UBTDecorator_CheckStateTag::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
 	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);
 	
	const APawn* Pawn = OwnerComp.GetAIOwner()->GetPawn();
	if (Pawn == nullptr)
	{
		return false;
	}
	
	const UStateTagComponent* StateTagComponent = Pawn->FindComponentByClass<UStateTagComponent>();
	
	if (StateTagComponent == nullptr)
	{
		return false;
	}
	
	switch (MatchType)
	{
	case EStateTagMatchType::Any:
		return StateTagComponent->HasAnyStateTags(RequiredStates);
	case EStateTagMatchType::All:
		return StateTagComponent->HasAllStateTags(RequiredStates);
	default:
		return false;
	}
}
