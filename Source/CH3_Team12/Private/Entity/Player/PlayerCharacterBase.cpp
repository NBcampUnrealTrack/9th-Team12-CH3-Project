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
#include "Entity/Player/PlayerEquipmentComponent.h"
#include "Entity/Player/PlayerInventoryComponent.h"

// Sets default values
APlayerCharacterBase::APlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// 아래는 임시 설정 이후에 컴포넌트에서 각자 알아서 조절해야함
	{
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;
	
		CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
		CameraBoom->SetupAttachment(RootComponent);

		FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
		FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	}
	
	StateTagComponent = CreateDefaultSubobject<UStateTagComponent>(TEXT("StateComponent"));
	LocomotionComponent = CreateDefaultSubobject<UPlayerLocomotionComponent>(TEXT("LocomotionComponent"));
	AttributeComponent = CreateDefaultSubobject<UPlayerAttributeComponent>(TEXT("AttributeComponent"));
	CombatComponent = CreateDefaultSubobject<UPlayerCombatComponent>(TEXT("CombatComponent"));
	CameraComponent = CreateDefaultSubobject<UPlayerCameraComponent>(TEXT("CameraComponent"));
	InventoryComponent = CreateDefaultSubobject<UPlayerInventoryComponent>(TEXT("InventoryComponent"));
	EquipmentComponent = CreateDefaultSubobject<UPlayerEquipmentComponent>(TEXT("EquipmentComponent"));
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

UPlayerInventoryComponent* APlayerCharacterBase::GetInventoryComponent() const
{
	return InventoryComponent;
}

UPlayerEquipmentComponent* APlayerCharacterBase::GetEquipmentComponent() const
{
	return EquipmentComponent;
}

UPlayerCameraComponent* APlayerCharacterBase::GetPlayerCameraComponent() const
{
	return CameraComponent;
}

USpringArmComponent* APlayerCharacterBase::GetCameraBoom() const
{
	return CameraBoom;
}

UCameraComponent* APlayerCharacterBase::GetFollowCamera() const
{
	return FollowCamera;
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
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, CameraComponent.Get(), &UPlayerCameraComponent::Look);
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
		EnhancedInputComponent->BindAction(SprintDodgeAction, ETriggerEvent::Started, LocomotionComponent.Get(), &UPlayerLocomotionComponent::OnSprintDodgePressed);
		EnhancedInputComponent->BindAction(SprintDodgeAction, ETriggerEvent::Completed, LocomotionComponent.Get(), &UPlayerLocomotionComponent::OnSprintDodgeReleased);
		EnhancedInputComponent->BindAction(SprintDodgeAction, ETriggerEvent::Canceled, LocomotionComponent.Get(), &UPlayerLocomotionComponent::OnSprintDodgeReleased);
	}
	
	// Combat
	if (UInputAction* AttackAction = PlayerControllerBase->GetAttackAction())
	{
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, CombatComponent.Get(), &UPlayerCombatComponent::Attack);
	}
	if (UInputAction* LockOnAction = PlayerControllerBase->GetLockOnAction())
	{
		EnhancedInputComponent->BindAction(LockOnAction, ETriggerEvent::Started, CameraComponent.Get(), &UPlayerCameraComponent::LockOn);
	}
	if (UInputAction* GuardAction = PlayerControllerBase->GetGuardAction())
	{
		EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Started, CombatComponent.Get(), &UPlayerCombatComponent::StartGuard);
		EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Completed, CombatComponent.Get(), &UPlayerCombatComponent::StopGuard);
		EnhancedInputComponent->BindAction(GuardAction, ETriggerEvent::Canceled, CombatComponent.Get(), &UPlayerCombatComponent::StopGuard);
	}
	
	// TestAction
	if (UInputAction* TestAction1 = PlayerControllerBase->GetTestAction1())
	{
		EnhancedInputComponent->BindAction(TestAction1, ETriggerEvent::Started, CombatComponent.Get(), &UPlayerCombatComponent::Debug_ReceiveTestAttackFront);
	}
	if (UInputAction* TestAction2 = PlayerControllerBase->GetTestAction2())
	{
		EnhancedInputComponent->BindAction(TestAction2, ETriggerEvent::Started, CombatComponent.Get(), &UPlayerCombatComponent::Debug_ReceiveTestAttackBack);
	}
	if (UInputAction* TestAction3 = PlayerControllerBase->GetTestAction3())
	{
		EnhancedInputComponent->BindAction(TestAction3, ETriggerEvent::Started, CombatComponent.Get(), &UPlayerCombatComponent::Debug_ReceiveTestAttackLeft);
	}
	if (UInputAction* TestAction4 = PlayerControllerBase->GetTestAction4())
	{
		EnhancedInputComponent->BindAction(TestAction4, ETriggerEvent::Started, CombatComponent.Get(), &UPlayerCombatComponent::Debug_ReceiveTestAttackRight);
	}
	
	if (UInputAction* EquipAction = PlayerControllerBase->GetEquipAction())
	{
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, EquipmentComponent.Get(), &UPlayerEquipmentComponent::ToggleWeaponInput);
	}
	
	// if (UInputAction* UseItemAction = PlayerControllerBase->GetUseItemAction())
	// {
	// 	EnhancedInputComponent->BindAction(UseItemAction, ETriggerEvent::Started, InventoryComponent.Get(), &UPlayerInventoryComponent::);
	// }
}

void APlayerCharacterBase::AttackAnimationEnd()
{
	ensureMsgf(CombatComponent, TEXT("Katana_PlayerCharacterBase. CombatComponent is invalid."));
	
	if (CombatComponent)
	{
		CombatComponent->EndAttack();
	}	
}

void APlayerCharacterBase::AttackHitCheckStart()
{
	ensureMsgf(CombatComponent, TEXT("Katana_PlayerCharacterBase. CombatComponent is invalid."));
	
	if (CombatComponent)
	{
		CombatComponent->StartWeaponHitCheck();
	}
}

void APlayerCharacterBase::AttackHitCheckTick()
{
	ensureMsgf(CombatComponent, TEXT("Katana_PlayerCharacterBase. CombatComponent is invalid."));
	
	if (CombatComponent)
	{
		CombatComponent->WeaponTrace();
	}
}

void APlayerCharacterBase::AttackHitCheckEnd()
{
	ensureMsgf(CombatComponent, TEXT("Katana_PlayerCharacterBase. CombatComponent is invalid."));
	
	if (CombatComponent)
	{
		CombatComponent->EndWeaponHitCheck();
	}
}


