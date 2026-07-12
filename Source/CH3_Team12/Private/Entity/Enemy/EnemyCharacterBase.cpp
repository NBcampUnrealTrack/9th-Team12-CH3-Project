// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/EnemyCharacterBase.h"

#include "Entity/Enemy/AI/EnemyAIController.h"
#include "Entity/Enemy/Component/EnemyAttackComponent.h"
#include "Engine/Engine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Entity/Enemy/Component/EnemyAttributeComponent.h"
#include "Entity/Player/StateTagComponent.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"

// Sets default values
AEnemyCharacterBase::AEnemyCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	EnemyAttackComponent = CreateDefaultSubobject<UEnemyAttackComponent>(TEXT("EnemyAttackComponent"));
	AttributeComponent = CreateDefaultSubobject<UEnemyAttributeComponent>(TEXT("AttributeComponent"));
	StateTagComponent = CreateDefaultSubobject<UStateTagComponent>(TEXT("StateTagComponent"));
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
	
	AttributeComponent->OnEnemyDeath.AddDynamic(this, &AEnemyCharacterBase::OnDeath);
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

void AEnemyCharacterBase::OnDeath()
{
	AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
	if (AIController)
	{
		AIController->StopMovement();
		if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement())
		{
			CharacterMovementComponent->DisableMovement();
		}
		UBrainComponent* BrainComponent = AIController->GetBrainComponent();
		if (BrainComponent)
		{
			BrainComponent->StopLogic(TEXT("Dead"));
		}
		
	}
	
	UCapsuleComponent* CollisionComponent = GetCapsuleComponent();
	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionProfileName(TEXT("NoCollision"));
	}
	
	EnemyAttackComponent->CancelAttack();
	GetWorldTimerManager().ClearAllTimersForObject(this);
	PlayDeathMontage();
}

void AEnemyCharacterBase::PlayDeathMontage()
{
	USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
	if (SkeletalMeshComponent)
	{
		UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
		if (AnimInstance == nullptr
			|| DeadMontage == nullptr)
		{
			SetEnemyDestroyTimer();
			return;
		}
		
		AnimInstance->Montage_Play(DeadMontage);
		SetEnemyDestroyTimer();
	}
}

void AEnemyCharacterBase::SetEnemyDestroyTimer()
{
	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}
	
	FTimerHandle DestroyHandle;
	FTimerManager& WorldTimerManager = World->GetTimerManager();
	WorldTimerManager.SetTimer(DestroyHandle, this, &AEnemyCharacterBase::DestroyEnemy, DestroyTime, false);	
}

void AEnemyCharacterBase::DestroyEnemy()
{
	Destroy();
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
