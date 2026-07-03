// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_StrafeAroundTarget.generated.h"

class AAIController;
class UCharacterMovementComponent;

UCLASS()
class CH3_TEAM12_API UBTTask_StrafeAroundTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_StrafeAroundTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

	UPROPERTY(EditAnywhere, Category="AI")
	FName TargetActorKeyName = TEXT("TargetActor");

	UPROPERTY(EditAnywhere, Category="AI")
	float StrafeDuration = 5.f;

	UPROPERTY(EditAnywhere, Category="AI")
	float RepathInterval = 0.5f;

	UPROPERTY(EditAnywhere, Category="AI")
	float DesiredDistance = 500.f;

	UPROPERTY(EditAnywhere, Category="AI")
	float StrafeOffset = 800.f;

	UPROPERTY(EditAnywhere, Category="AI")
	float StrafeMoveSpeed = 300.f;

	UPROPERTY(EditAnywhere, Category="AI")
	float AfterStrafeMoveSpeed = 600.f;

	UPROPERTY(EditAnywhere, Category="AI")
	float AcceptanceRadius = 50.f;

private:
	bool RequestStrafeMove(UBehaviorTreeComponent& OwnerComp);
	void FaceTarget(UBehaviorTreeComponent& OwnerComp, float DeltaSeconds) const;
	void RestoreMoveSpeed() const;

	UPROPERTY()
	TObjectPtr<AAIController> CachedAIController;

	UPROPERTY()
	TObjectPtr<APawn> CachedControlledPawn;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> CachedMovementComponent;

	float ElapsedTime = 0.f;
	float RepathElapsedTime = 0.f;
	float SideSign = 1.f;
};
