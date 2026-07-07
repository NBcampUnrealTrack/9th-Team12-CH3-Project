// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/Component/EnemyAttackComponent.h"

#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"

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

bool UEnemyAttackComponent::ExecuteAttack(AActor* TargetActor, int32 SelectedAction, int32 SelectedPattern)
{
	UE_LOG(LogTemp, Warning, TEXT("ExecuteAttack: Action=%d Pattern=%d"), SelectedAction, SelectedPattern);
	AActor* OwnerActor = GetOwner();

	if (!OwnerActor || !TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteAttack Failed: OwnerActor or TargetActor is null"));
		return false;
	}
	const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteAttack Failed: OwnerCharacter is null"));
		return false;
	}

	if (!bCanAttack)
	{
		return false;
	}

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteAttack Failed: Animinstance"));
		return false;
	}

	// if (SelectedAction == 1)
	// {
	// 	// TODO: 일반공격
	// 	// NOTE: 공격Montage 끝나면 OnAttackFinished.Broadcast() 호출
	// 	UE_LOG(LogTemp, Log, TEXT("Execute Normal Attack Pattern: %d"), SelectedPattern);
	// }
	// else if (SelectedAction == 2)
	// {
	// 	// TODO: 강공격
	// 	// NOTE: 공격Montage 끝나면 OnAttackFinished.Broadcast() 호출
	// 	UE_LOG(LogTemp, Log, TEXT("Execute Strong Attack Pattern: %d"), SelectedPattern);
	// }
	UAnimMontage* SelectedMontage = nullptr;

	if (SelectedAction == 1 && SelectedPattern == 0)
	{
		// NormalAttackMontage0 
		SelectedMontage = NormalAttack0;
	}
	else if (SelectedAction == 1 && SelectedPattern == 1)
	{
		// NormalAttackMontage1 
		SelectedMontage = NormalAttack0;
	}
	else if (SelectedAction == 2 && SelectedPattern == 0)
	{
		// StrongAttackMontage0 
		SelectedMontage = NormalAttack0;
	}
	else if (SelectedAction == 2 && SelectedPattern == 1)
	{
		// FarStrongAttackMontage 재생
		SelectedMontage = FarStrongAttack;
	}


	const float DistanceToTarget = FVector::Dist(
		OwnerActor->GetActorLocation(),
		TargetActor->GetActorLocation()
	);

	if (DistanceToTarget > AttackRange)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteAttack Failed: RangeShort"));
		return false;
	}
	if (!SelectedMontage)
	{
		return false;
	}


	GetWorld()->GetTimerManager().SetTimer(
		AttackCooldownTimerHandle,
		this,
		&UEnemyAttackComponent::ResetAttackCooldown,
		AttackCooldown,
		false
	);

	const float MontageLength = AnimInstance->Montage_Play(SelectedMontage);

	AnimInstance->Montage_SetNextSection(

		TEXT("Attack1"),
		TEXT("Attack2"),
		SelectedMontage
	);
	AnimInstance->Montage_SetNextSection(
		TEXT("Attack2"),
		TEXT("Attack3"),
		SelectedMontage
	);
	UE_LOG(LogTemp, Warning, TEXT(" MontageLength: %f"), MontageLength);
	if (MontageLength <= 0.f)
	{
		return false;
	}


	bCanAttack = false;
	return true;
}

void UEnemyAttackComponent::ResetAttackCooldown()
{
	bCanAttack = true;
}

void UEnemyAttackComponent::FinishAttack()
{
	bCanAttack = true;
	OnAttackFinished.Broadcast();
}

void UEnemyAttackComponent::StopAttackMontage()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	AnimInstance->Montage_Stop(0.15f);
	FinishAttack();
}
