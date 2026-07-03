// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SelectStrafeLocation.generated.h"

/**
 * 
 */
UCLASS()
class CH3_TEAM12_API UBTTask_SelectStrafeLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SelectStrafeLocation();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere, Category="AI")
	FName TargetActorKeyName = TEXT("TargetActor");

	UPROPERTY(EditAnywhere, Category="AI")
	FName StrafeLocationKeyName = TEXT("StrafeLocation");

	UPROPERTY(EditAnywhere, Category="AI")
	float DesiredDistance = 500.f;

	UPROPERTY(EditAnywhere, Category="AI")
	float StrafeOffset = 800.f;
};
