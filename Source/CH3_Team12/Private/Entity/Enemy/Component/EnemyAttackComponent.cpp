// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/Component/EnemyAttackComponent.h"

#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

// Sets default values for this component's properties
UEnemyAttackComponent::UEnemyAttackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UEnemyAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}

bool UEnemyAttackComponent::ExecuteAttack(AActor* TargetActor)
{
	AActor* OwnerActor = GetOwner();

	if (!OwnerActor || !TargetActor)
	{
		return false;
	}

	if (!bCanAttack)
	{
		return false;
	}

	const float DistanceToTarget = FVector::Dist(
		OwnerActor->GetActorLocation(),
		TargetActor->GetActorLocation()
	);

	if (DistanceToTarget > AttackRange)
	{
		return false;
	}

	bCanAttack = false;

	// NOTE: 디버그
	DrawDebugSphere(
		GetWorld(),
		TargetActor->GetActorLocation(),
		50.f,
		16,
		FColor::Red,
		false,
		1.f
	);

	DrawDebugLine(
		GetWorld(),
		OwnerActor->GetActorLocation(),
		TargetActor->GetActorLocation(),
		FColor::Red,
		1.f,
		0,
		2.f
	);

	GetWorld()->GetTimerManager().SetTimer(
		AttackCooldownTimerHandle,
		this,
		&UEnemyAttackComponent::ResetAttackCooldown,
		AttackCooldown,
		false
	);

	return true;
}

void UEnemyAttackComponent::ResetAttackCooldown()
{
	bCanAttack = true;
}
