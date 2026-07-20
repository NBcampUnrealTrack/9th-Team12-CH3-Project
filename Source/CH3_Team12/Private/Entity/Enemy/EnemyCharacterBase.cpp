// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/EnemyCharacterBase.h"

#include "Entity/Enemy/AI/EnemyAIController.h"
#include "Entity/Enemy/Component/EnemyAttackComponent.h"
#include "Entity/Enemy/Component/EnemyDefenseComponent.h"
#include "Entity/Enemy/Component/EnemyTransitionComponent.h"
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
#include "GameplayTags/CombatGameplayTags.h"
#include "NiagaraSystem.h"
#include "Framework/DataAsset/EnemyExecutionDataAsset.h"

// Sets default values
AEnemyCharacterBase::AEnemyCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	Tags.Add(TEXT("Enemy"));

	AIControllerClass = AEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	EnemyAttackComponent = CreateDefaultSubobject<UEnemyAttackComponent>(TEXT("EnemyAttackComponent"));
	AttributeComponent = CreateDefaultSubobject<UEnemyAttributeComponent>(TEXT("AttributeComponent"));
	EnemyDefenseComponent = CreateDefaultSubobject<UEnemyDefenseComponent>(TEXT("EnemyDefenseComponent"));
	EnemyTransitionComponent = CreateDefaultSubobject<UEnemyTransitionComponent>(TEXT("EnemyTransitionComponent"));
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

	if (AttributeComponent)
	{
		AttributeComponent->OnEnemyDeath.AddDynamic(this, &AEnemyCharacterBase::OnDeath);
		AttributeComponent->OnEnemyPostureBroken.AddDynamic(this, &AEnemyCharacterBase::HandlePostureBroken);
	}
	
	if (EnemyTransitionComponent)
	{
		DestroyTime = EnemyTransitionComponent->GetDissolveDuration() + 15;
	}
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

float AEnemyCharacterBase::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
                                      AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 임의로 체간 게이지는 두배로 받도록 설정
	AttributeComponent->ApplyAttributeDamage(DamageAmount, DamageAmount * 2);

	return DamageAmount;
}

void AEnemyCharacterBase::HandlePostureBroken()
{
	EnemyAttackComponent->CancelAttack();
	GetWorldTimerManager().ClearAllTimersForObject(this);
	StopAILogic();

	PlayGroggyMontage();
}

void AEnemyCharacterBase::PlayGroggyMontage()
{
	USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
	if (SkeletalMeshComponent)
	{
		UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
		if (AnimInstance == nullptr
			|| GroggyMontage == nullptr)
		{
			return;
		}

		FOnMontageEnded MontageEndedDelegate;
		MontageEndedDelegate.BindUObject(
			this, &AEnemyCharacterBase::OnGroggyMontageEnded);

		AnimInstance->Montage_Play(GroggyMontage);
		AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, GroggyMontage);
	}
}

void AEnemyCharacterBase::OnGroggyMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	StateTagComponent->RemoveStateTag(CombatTags::State_Hit_PostureBroken);
	if (StateTagComponent->HasStateTagExact(CombatTags::State_Action_Executing) == false)
		ResumeAILogic();
}

bool AEnemyCharacterBase::CanExecuted()
{
	const UEnemyExecutionDataAsset* EnemyData = GetExecutionData();

	bool Result = EnemyData->EnemyExecutionData.EnemyMontage && EnemyData;
	
	ensure(EnemyData);  
	ensure(EnemyData->EnemyExecutionData.EnemyMontage);

	return Result;
}

void AEnemyCharacterBase::StartExecuted()
{
	if (CanExecuted())
	{
		StartExecuted_Implement();	
	}
}

void AEnemyCharacterBase::OnDeath()
{
	StopAILogic();

	UCapsuleComponent* CollisionComponent = GetCapsuleComponent();
	if (CollisionComponent)
	{
		CollisionComponent->SetCollisionProfileName(TEXT("NoCollision"));
	}

	EnemyAttackComponent->CancelAttack();
	GetWorldTimerManager().ClearAllTimersForObject(this);
	bool Result = PlayDeathMontage();
	if (Result == false)
	{
		if (USkeletalMeshComponent* SkeletalMesh = GetMesh())
		{
			SkeletalMesh->SetHiddenInGame(true);
		}
	}	
	StartDeathTransition();
	SetEnemyDestroyTimer();
}


bool AEnemyCharacterBase::PlayDeathMontage()
{
	USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
	if (!SkeletalMeshComponent)
	{
		return false;
	}

	UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
	if (!AnimInstance || !DeadMontage)
	{
		return false;
	}

	AnimInstance->Montage_Play(DeadMontage);
	
	return true;
}

void AEnemyCharacterBase::StartDeathTransition()
{
	UE_LOG(LogTemp, Warning, TEXT("Enemy Death Transition Start / Mesh: %s / VFX: %s"),
	       *GetNameSafe(GetMesh()),
	       *GetNameSafe(DeathDisintegrationVFX.Get()));

	if (EnemyTransitionComponent)
	{
		EnemyTransitionComponent->PlayDeathTransition(GetMesh(), DeathDisintegrationVFX);
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
	OnDestroyedDelegate.Broadcast();
	Destroy();
}

void AEnemyCharacterBase::StopAILogic()
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
}

void AEnemyCharacterBase::ResumeAILogic()
{
	AEnemyAIController* AIController = Cast<AEnemyAIController>(GetController());
	if (AIController)
	{
		if (UCharacterMovementComponent* CharacterMovementComponent = GetCharacterMovement())
		{
			CharacterMovementComponent->SetMovementMode(MOVE_Walking);
		}
		UBrainComponent* BrainComponent = AIController->GetBrainComponent();
		if (BrainComponent)
		{
			BrainComponent->RestartLogic();
		}
	}
}

void AEnemyCharacterBase::StartExecuted_Implement()
{
	if (StateTagComponent)
	{
		StateTagComponent->AddStateTag(CombatTags::State_Action_Executing);
		
		StopAILogic();
		if (!PlayExecutedMontage())
		{
			OnExecutedMontageEnded(nullptr, false);
		}
	}	
}

bool AEnemyCharacterBase::PlayExecutedMontage()
{
	USkeletalMeshComponent* SkeletalMeshComponent = GetMesh();
	if (!SkeletalMeshComponent)
	{
		return false;
	}

	UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
	if (!AnimInstance || !ExecutionData)
	{
		return false;
	}

	UAnimMontage* ExecutedMontage = ExecutionData->EnemyExecutionData.EnemyMontage;
	if (ExecutedMontage)
	{
		FOnMontageEnded OnMontageEndedDelegate;
		OnMontageEndedDelegate.BindUObject(
				this, &AEnemyCharacterBase::OnExecutedMontageEnded);
		
		AnimInstance->Montage_Play(ExecutedMontage);
		AnimInstance->Montage_SetEndDelegate(OnMontageEndedDelegate, ExecutedMontage);
		
		return true;
	}

	return false;
}

void AEnemyCharacterBase::OnExecutedMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	StateTagComponent->RemoveStateTag(CombatTags::State_Action_Executing);
	ResumeAILogic();
}

void AEnemyCharacterBase::AttackAnimationEnd()
{
	ensureMsgf(EnemyAttackComponent, TEXT("Katana_EnemyCharacterBase. EnemyAttackComponent is invalid."));

	if (EnemyAttackComponent)
	{
		EnemyAttackComponent->FinishAttack();
	}
}

void AEnemyCharacterBase::AttackHitCheckStart(int32 HitIndex, const TArray<FHitBoxData>& HitBoxes)
{
	ensureMsgf(EnemyAttackComponent, TEXT("Katana_EnemyCharacterBase. AttackComponent is invalid."));

	if (EnemyAttackComponent)
	{
		EnemyAttackComponent->StartHitCheck(HitBoxes);
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
