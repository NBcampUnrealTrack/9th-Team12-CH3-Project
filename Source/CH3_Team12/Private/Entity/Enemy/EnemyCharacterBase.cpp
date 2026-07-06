// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/EnemyCharacterBase.h"

#include "Entity/Enemy/AI/EnemyAIController.h"
#include "Entity/Enemy/Component/EnemyAttackComponent.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AEnemyCharacterBase::AEnemyCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	EnemyAttackComponent = CreateDefaultSubobject<UEnemyAttackComponent>(TEXT("EnemyAttackComponent"));
}

// Called when the game starts or when spawned
void AEnemyCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEnemyCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	if (!PlayerPawn || !GEngine)
	{
		return;
	}

	const float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerPawn->GetActorLocation());
	GEngine->AddOnScreenDebugMessage(
		1,
		0.f,
		FColor::Green,
		FString::Printf(TEXT("Enemy Distance To Player: %.2f"), DistanceToPlayer)
	);
}

// Called to bind functionality to input
void AEnemyCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemyCharacterBase::AttackAnimationEnd()
{
	ensureMsgf(EnemyAttackComponent, TEXT("Katana_EnemyCharacterBase. EnemyAttackComponent is invalid."));
	
	if (EnemyAttackComponent)
	{
		EnemyAttackComponent->FinishAttack();	
	}
}

void AEnemyCharacterBase::AttackHitCheckStart()
{
	UE_LOG(LogTemp, Log, TEXT("Enemy Attack Trace Start"));
}

void AEnemyCharacterBase::AttackHitCheckTick()
{
	UE_LOG(LogTemp, Log, TEXT("Enemy Attack Trace Tick"));
}

void AEnemyCharacterBase::AttackHitCheckEnd()
{
	UE_LOG(LogTemp, Log, TEXT("Enemy Attack Trace End"));
}
