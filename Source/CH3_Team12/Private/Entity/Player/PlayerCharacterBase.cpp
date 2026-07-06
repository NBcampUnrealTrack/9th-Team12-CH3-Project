// Fill out your copyright notice in the Description page of Project Settings.

#include "Entity/Player/PlayerCharacterBase.h"

#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Entity/Player/PlayerControllerBase.h"
#include "Entity/Player/StateTagComponent.h"
#include "Entity/Player/PlayerLocomotionComponent.h"
#include "Entity/Player/PlayerAttributeComponent.h"
#include "Entity/Player/PlayerCameraComponent.h"
#include "Entity/Player/PlayerCombatComponent.h"

// Sets default values
APlayerCharacterBase::APlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// 아래는 임시 설정 이후에 컴포넌트에서 각자 알아서 조절해야함
	{
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;
	
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 1000.0f, 0.0f); // 회전 속도
		// GetCharacterMovement()->MaxWalkSpeed = RunSpeed;
		GetCharacterMovement()->MaxAcceleration = 4096.0f;
		GetCharacterMovement()->GroundFriction = 4.0f;
		GetCharacterMovement()->BrakingDecelerationWalking = 200.0f;
		GetCharacterMovement()->GravityScale = 1.0f;
	
		CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
		CameraBoom->SetupAttachment(RootComponent);
		CameraBoom->TargetArmLength = 400.0f;
		CameraBoom->bUsePawnControlRotation = true;
		CameraBoom->bEnableCameraLag = true;
		CameraBoom->CameraLagSpeed = 4.0f;
		CameraBoom->CameraLagMaxDistance = 200.0f;
		CameraBoom->bEnableCameraRotationLag = true;
		CameraBoom->CameraRotationLagSpeed = 12.0f;
		CameraBoom->bDoCollisionTest = false;

		FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
		FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
		FollowCamera->bUsePawnControlRotation = false; // 카메라는 암의 회전을 따라가기만 함
		
	}
	
	StateTagComponent = CreateDefaultSubobject<UStateTagComponent>(TEXT("StateComponent"));
	LocomotionComponent = CreateDefaultSubobject<UPlayerLocomotionComponent>(TEXT("LocomotionComponent"));
	AttributeComponent = CreateDefaultSubobject<UPlayerAttributeComponent>(TEXT("AttributeComponent"));
	CombatComponent = CreateDefaultSubobject<UPlayerCombatComponent>(TEXT("CombatComponent"));
}

UStateTagComponent* APlayerCharacterBase::GetStateTagComponent() const
{
	return StateTagComponent;
}

UPlayerLocomotionComponent* APlayerCharacterBase::GetLocomotionComponent() const
{
	return LocomotionComponent;
}

UPlayerAttributeComponent* APlayerCharacterBase::GetAttributeComponent() const
{
	return AttributeComponent;
}

UPlayerCombatComponent* APlayerCharacterBase::GetCombatComponent() const
{
	return CombatComponent;
}

// Called when the game starts or when spawned
void APlayerCharacterBase::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void APlayerCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (!EnhancedInputComponent)
	{
		return;
	}

	APlayerControllerBase* PlayerControllerBase = Cast<APlayerControllerBase>(GetController());
	if (!PlayerControllerBase) return;

	// Locomotion
	if (UInputAction* LookAction = PlayerControllerBase->GetLookAction())
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, LocomotionComponent.Get(), &UPlayerLocomotionComponent::Look);
	}
	if (UInputAction* MoveAction = PlayerControllerBase->GetMoveAction())
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, LocomotionComponent.Get(), &UPlayerLocomotionComponent::DoMove);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, LocomotionComponent.Get(), &UPlayerLocomotionComponent::DoStopMove);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, LocomotionComponent.Get(), &UPlayerLocomotionComponent::DoStopMove);
	}
	if (UInputAction* JumpAction = PlayerControllerBase->GetJumpAction())
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, LocomotionComponent.Get(), &UPlayerLocomotionComponent::DoStartJump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, LocomotionComponent.Get(), &UPlayerLocomotionComponent::DoStopJump);
	}
	if (UInputAction* SprintDodgeAction = PlayerControllerBase->GetSprintDodgeAction())
	{
		EnhancedInputComponent->BindAction(SprintDodgeAction, ETriggerEvent::Triggered, LocomotionComponent.Get(), &UPlayerLocomotionComponent::DoStartSprint);
		EnhancedInputComponent->BindAction(SprintDodgeAction, ETriggerEvent::Completed, LocomotionComponent.Get(), &UPlayerLocomotionComponent::DoStopSprint);
		EnhancedInputComponent->BindAction(SprintDodgeAction, ETriggerEvent::Canceled, CombatComponent.Get(), &UPlayerCombatComponent::Dodge);
	}
	
	// Combat
	if (UInputAction* AttackAction = PlayerControllerBase->GetAttackAction())
	{
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, CombatComponent.Get(), &UPlayerCombatComponent::Attack);
	}
	if (UInputAction* LockOnAction = PlayerControllerBase->GetLockOnAction())
	{
		// EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Started, CameraComponent.Get(), &UPlayerCameraComponent::LockOn);
	}
	if (UInputAction* ParryAction = PlayerControllerBase->GetParryingAction())
	{
		// EnhancedInputComponent->BindAction(ParryAction, ETriggerEvent::Started, CombatComponent.Get(), &UPlayerCombatComponent::Parry);
	}
}


