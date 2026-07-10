// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/EnemyCharacterBase.h"

#include "Entity/Enemy/AI/EnemyAIController.h"
#include "Entity/Enemy/Component/EnemyAttackComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Entity/Enemy/Component/EnemyAttributeComponent.h"

// Sets default values
AEnemyCharacterBase::AEnemyCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	EnemyAttackComponent = CreateDefaultSubobject<UEnemyAttackComponent>(TEXT("EnemyAttackComponent"));
	AttributeComponent = CreateDefaultSubobject<UEnemyAttributeComponent>(TEXT("AttributeComponent"));
}

// Called when the game starts or when spawned
void AEnemyCharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// bUseControllerRotationPitch = false;
	// bUseControllerRotationYaw = false;
	// bUseControllerRotationRoll = false;
	//
	// // UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	// // if (MovementComponent)
	// // {
	// // 	MovementComponent->bOrientRotationToMovement = true;
	// // 	MovementComponent->bUseControllerDesiredRotation = false;
	// // 	MovementComponent->RotationRate = FRotator(0.f, 720.f, 0.f);
	// // }
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

float AEnemyCharacterBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	// 임의로 체간 게이지는 두배로 받도록 설정
	AttributeComponent->ApplyAttributeDamage(DamageAmount, DamageAmount * 2);
	
	return DamageAmount;
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
	ensureMsgf(EnemyAttackComponent, TEXT("Katana_EnemyCharacterBase. AttackComponent is invalid."));
	
	if (EnemyAttackComponent)
	{
		EnemyAttackComponent->StartHitCheck();
	}
}

void AEnemyCharacterBase::AttackHitCheckTick()
{
	ensureMsgf(EnemyAttackComponent, TEXT("Katana_EnemyCharacterBase. AttackComponent is invalid."));
	
	if (EnemyAttackComponent)
	{
		EnemyAttackComponent->AttackTrace();
	}
}

void AEnemyCharacterBase::AttackHitCheckEnd()
{
	ensureMsgf(EnemyAttackComponent, TEXT("Katana_EnemyCharacterBase. AttackComponent is invalid."));
	
	if (EnemyAttackComponent)
	{
		EnemyAttackComponent->EndHitCheck();
	}
}
