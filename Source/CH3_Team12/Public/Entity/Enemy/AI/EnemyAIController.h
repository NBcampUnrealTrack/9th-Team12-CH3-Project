// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "EnemyAIController.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnTargetActorSetted);

class UBehaviorTreeComponent;
/**
 * 
 */
UCLASS()
class CH3_TEAM12_API AEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyAIController();

	virtual void BeginPlay() override;
	
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	AActor* GetTargetActor() const {return TargetActor;}
	
	FOnTargetActorSetted OnTargetActorSettedDelegate;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	TObjectPtr<UBlackboardComponent> BlackboardComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	FName TargetActorKeyName = TEXT("TargetActor");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	FName HomeLocationKeyName = TEXT("HomeLocation");

	FTimerHandle TargetActorTimerHandle;
	void UpdateTargetActor();
	
private:
	UFUNCTION()
	void OnPlayerEvadeSuccess(const FIncomingAttackContext& InAttackContext);
	
	UPROPERTY()
	TObjectPtr<AActor> TargetActor = nullptr;
};
