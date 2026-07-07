// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/AI/BTTask_StrafeAroundTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"

UBTTask_StrafeAroundTarget::UBTTask_StrafeAroundTarget()
{
	NodeName = TEXT("Strafe Around Target");
	bCreateNodeInstance = true;
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTask_StrafeAroundTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	CachedAIController = OwnerComp.GetAIOwner();
	if (!CachedAIController)
	{
		return EBTNodeResult::Failed;
	}

	CachedControlledPawn = CachedAIController->GetPawn();
	if (!CachedControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	ACharacter* ControlledCharacter = Cast<ACharacter>(CachedControlledPawn);
	if (!ControlledCharacter)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	BlackboardComponent->SetValueAsBool(TEXT("bIsStrafing"), true);

	CachedMovementComponent = ControlledCharacter->GetCharacterMovement();
	if (!CachedMovementComponent)
	{
		return EBTNodeResult::Failed;
	}

	CachedMovementComponent->MaxWalkSpeed = StrafeMoveSpeed;

	ElapsedTime = 0.f;
	RepathElapsedTime = 0.f;
	SideSign = FMath::RandBool() ? 1.f : -1.f;

	if (!RequestStrafeMove(OwnerComp))
	{
		RestoreMoveSpeed();
		return EBTNodeResult::Failed;
	}

	CachedMovementComponent->bOrientRotationToMovement = false;
	CachedMovementComponent->bUseControllerDesiredRotation = false;

	return EBTNodeResult::InProgress;
}

void UBTTask_StrafeAroundTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ElapsedTime += DeltaSeconds;
	RepathElapsedTime += DeltaSeconds;

	// FaceTarget(OwnerComp, DeltaSeconds);

	if (ElapsedTime >= StrafeDuration)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}

	if (RepathElapsedTime >= RepathInterval)
	{
		RepathElapsedTime = 0.f;
		RequestStrafeMove(OwnerComp);
	}

	const FVector Velocity = CachedControlledPawn->GetVelocity();
	FVector MoveDirection = Velocity;

	MoveDirection.Z = 0.f;

	if (!MoveDirection.IsNearlyZero())
	{
		const FRotator TargetRotation = MoveDirection.Rotation();
		const FRotator CurrentRotation = CachedControlledPawn->GetActorRotation();

		const FRotator NewRotation = FMath::RInterpTo(
			CurrentRotation,
			FRotator(0.f, TargetRotation.Yaw, 0.f),
			DeltaSeconds,
			BodyRotationInterpSpeed
		);

		CachedControlledPawn->SetActorRotation(NewRotation);
	}
}

void UBTTask_StrafeAroundTarget::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory
                                                , EBTNodeResult::Type TaskResult)
{
	RestoreMoveSpeed();

	CachedAIController = nullptr;
	CachedControlledPawn = nullptr;
	CachedMovementComponent = nullptr;

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent)
	{
		return;
	}

	BlackboardComponent->SetValueAsBool(TEXT("bIsStrafing"), false);

	CachedMovementComponent->bOrientRotationToMovement = true;
	CachedMovementComponent->bUseControllerDesiredRotation = false;

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

bool UBTTask_StrafeAroundTarget::RequestStrafeMove(UBehaviorTreeComponent& OwnerComp)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent || !CachedAIController || !CachedControlledPawn)
	{
		return false;
	}

	AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorKeyName));
	if (!TargetActor)
	{
		return false;
	}

	const FVector EnemyLocation = CachedControlledPawn->GetActorLocation();
	const FVector TargetLocation = TargetActor->GetActorLocation();

	FVector DirectionFromTarget = EnemyLocation - TargetLocation;
	DirectionFromTarget.Z = 0.f;

	if (DirectionFromTarget.IsNearlyZero())
	{
		DirectionFromTarget = -TargetActor->GetActorForwardVector();
		DirectionFromTarget.Z = 0.f;
	}

	DirectionFromTarget.Normalize();


	// 플레이어 중심
	// 현재 거리 반지름
	// 좌/우 각도만 회전

	const float CurrentDistance = FVector::Dist2D(EnemyLocation, TargetLocation);

	const float AngleDegree = StrafeOffset / FMath::Max(CurrentDistance, 1.f) * 57.29578f;
	const float SignedAngleDegree = AngleDegree * SideSign;

	const FVector RotatedDirection = DirectionFromTarget.RotateAngleAxis(SignedAngleDegree, FVector::UpVector);

	const FVector StrafeLocation =
		TargetLocation +
		RotatedDirection * CurrentDistance;

	const EPathFollowingRequestResult::Type MoveResult = CachedAIController->MoveToLocation(
		StrafeLocation,
		AcceptanceRadius,
		true,
		true,
		true,
		false,
		nullptr,
		true
	);

	return MoveResult != EPathFollowingRequestResult::Failed;
}

void UBTTask_StrafeAroundTarget::FaceTarget(UBehaviorTreeComponent& OwnerComp, float DeltaSeconds) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent || !CachedControlledPawn)
	{
		return;
	}

	AActor* TargetActor = Cast<AActor>(BlackboardComponent->GetValueAsObject(TargetActorKeyName));
	if (!TargetActor)
	{
		return;
	}

	const FVector Direction = TargetActor->GetActorLocation() - CachedControlledPawn->GetActorLocation();
	const FRotator TargetRotation = Direction.Rotation();
	const FRotator CurrentRotation = CachedControlledPawn->GetActorRotation();

	const FRotator NewRotation = FMath::RInterpTo(
		CurrentRotation,
		FRotator(0.f, TargetRotation.Yaw, 0.f),
		DeltaSeconds,
		20.f
	);

	CachedControlledPawn->SetActorRotation(NewRotation);
}

void UBTTask_StrafeAroundTarget::RestoreMoveSpeed() const
{
	if (CachedMovementComponent)
	{
		CachedMovementComponent->MaxWalkSpeed = AfterStrafeMoveSpeed;
	}
}
