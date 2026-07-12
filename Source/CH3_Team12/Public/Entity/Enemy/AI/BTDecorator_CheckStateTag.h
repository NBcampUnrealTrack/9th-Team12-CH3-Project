// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CheckStateTag.generated.h"

UENUM(BlueprintType)
enum class EStateTagMatchType : uint8
{
	Any UMETA(DisplayName = "Any (Or)"),
	All UMETA(DisplayName = "All (And)")
};

/**
 * 
 */
UCLASS()
class CH3_TEAM12_API UBTDecorator_CheckStateTag : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTDecorator_CheckStateTag();
	
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
	UPROPERTY(EditAnywhere, Category = "State")
	FGameplayTagContainer RequiredStates;
	
	UPROPERTY(EditAnywhere, Category = "State")
	EStateTagMatchType MatchType = EStateTagMatchType::Any;
	
};
