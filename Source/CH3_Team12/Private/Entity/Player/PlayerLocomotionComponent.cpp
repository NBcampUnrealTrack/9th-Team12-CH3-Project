#include "Entity/Player/PlayerLocomotionComponent.h"
#include "InputActionValue.h"
#include "GameplayTags/CombatGameplayTags.h"
#include "Entity/Player/StateTagComponent.h"
#include "Entity/Player/PlayerAttackComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerCameraComponent.h"
#include "Entity/Player/PlayerDefenseComponent.h"

#include "GameFramework/CharacterMovementComponent.h"

UPlayerLocomotionComponent::UPlayerLocomotionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	NormalWalkSpeed = 200.0f;
	SprintSpeed = 400.0f;
}

void UPlayerLocomotionComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<APlayerCharacterBase>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerLocomotionComponent: OwnerCharacter is nullptr"));
		return;
	}

	StateComponent = OwnerCharacter->GetStateTagComponent();
	MovementComponent = OwnerCharacter->GetCharacterMovement();
	CombatComponent = OwnerCharacter->GetCombatComponent();
	DefenseComponent = OwnerCharacter->GetDefenseComponent();
	
	{
		MovementComponent->bOrientRotationToMovement = true;
		MovementComponent->bUseControllerDesiredRotation = false;
		MovementComponent->RotationRate = FRotator(0.0f, 1000.0f, 0.0f);
		MovementComponent->MaxAcceleration = 2048.0f;
		MovementComponent->GroundFriction = 4.0f;
		MovementComponent->BrakingDecelerationWalking = 200.0f;
		MovementComponent->GravityScale = 1.0f;
	}
	
	{
		// Speed
		NormalWalkSpeed = LocomotionData->NormalWalkSpeed;
		WalkSpeed = LocomotionData->WalkSpeed;
		SprintSpeed = LocomotionData->SprintSpeed;
		LockOnWalkSpeed = LocomotionData->LockOnWalkSpeed;
		GuardWalkSpeed = LocomotionData->GuardWalkSpeed;
	
		// Dodge
		SprintHoldThreshold = LocomotionData->SprintHoldThreshold;
		DodgeBufferDuration = LocomotionData->DodgeBufferDuration;
		DodgeBlendOutTime = LocomotionData->DodgeBlendOutTime;
		
	}
	
	RefreshMovementSettings();
}

FVector UPlayerLocomotionComponent::GetDodgeWorldDirectionFromLastInput() const
{
	if (!OwnerCharacter)
	{
		return FVector::ForwardVector;
	}

	if (LastMovementInput.IsNearlyZero())
	{
		return OwnerCharacter->GetActorForwardVector();
	}

	// LockOn 중이면 타겟 기준 방향 사용
	if (StateComponent &&
		StateComponent->HasStateTagExact(CombatTags::State_Movement_LockOn))
	{
		if (UPlayerCameraComponent* PlayerCameraComponent =
			OwnerCharacter->GetPlayerCameraComponent())
		{
			if (AActor* LockOnTarget =
				PlayerCameraComponent->GetCurrentLockOnTarget())
			{
				FVector ForwardToTarget =
					LockOnTarget->GetActorLocation() -
					OwnerCharacter->GetActorLocation();

				ForwardToTarget.Z = 0.0f;
				ForwardToTarget.Normalize();

				const FVector Right =
					FRotationMatrix(ForwardToTarget.Rotation())
					.GetUnitAxis(EAxis::Y);

				FVector DodgeDirection =
					ForwardToTarget * LastMovementInput.X +
					Right * LastMovementInput.Y;

				DodgeDirection.Z = 0.0f;

				return DodgeDirection.GetSafeNormal();
			}
		}
	}

	// Normal camera 기준 방향
	const FRotator ControlRotation = OwnerCharacter->GetControlRotation();
	const FRotator YawRotation(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector Forward =
		FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	const FVector Right =
		FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	FVector DodgeDirection =
		Forward * LastMovementInput.X +
		Right * LastMovementInput.Y;

	DodgeDirection.Z = 0.0f;

	return DodgeDirection.GetSafeNormal();
}
void UPlayerLocomotionComponent::OpenDodgeRecovery()
{
	if (!OwnerCharacter || !StateComponent)
	{
		return;
	}

	StateComponent->RemoveStateTag(
		CombatTags::State_Movement_Locked
	);

	RefreshMovementSettings();

	if (LocomotionData->DodgeMontage)
	{
		if (UAnimInstance* AnimInstance =
			OwnerCharacter->GetMesh()->GetAnimInstance())
		{
			AnimInstance->Montage_Stop(
				DodgeBlendOutTime,
				LocomotionData->DodgeMontage
			);
		}
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DodgeEndTimerHandle);

		World->GetTimerManager().SetTimer(
			DodgeEndTimerHandle,
			this,
			&UPlayerLocomotionComponent::EndDodge,
			DodgeBlendOutTime,
			false
		);
	}
}

void UPlayerLocomotionComponent::RefreshMovementSettings()
{
	if (!MovementComponent || !StateComponent)
	{
		return;
	}

	const bool bIsGuarding =
		StateComponent->HasStateTagExact(
			CombatTags::State_Combat_Guarding
		);

	const bool bIsSprinting =
		StateComponent->HasStateTagExact(
			CombatTags::State_Movement_Sprinting
		);

	const bool bIsLockedOn =
		StateComponent->HasStateTagExact(
			CombatTags::State_Movement_LockOn
		);

	const bool bIsDodging =
		StateComponent->HasStateTagExact(
			CombatTags::State_Combat_Dodging
		);

	const bool bIsMovementLocked =
		StateComponent->HasStateTagExact(
			CombatTags::State_Movement_Locked
		);

	// =========================
	// Speed
	// =========================
	if (bIsGuarding)
	{
		MovementComponent->MaxWalkSpeed = GuardWalkSpeed;
	}
	else if (bIsSprinting)
	{
		MovementComponent->MaxWalkSpeed = SprintSpeed;
	}
	else if (bIsLockedOn)
	{
		MovementComponent->MaxWalkSpeed = LockOnWalkSpeed;
	}
	else
	{
		MovementComponent->MaxWalkSpeed = NormalWalkSpeed;
	}

	// =========================
	// Rotation
	// =========================
	// 락온/가드 중에는 기본적으로 컨트롤러 방향 기준 스트레이프.
	// 단, Dodge 중에는 Root Motion 방향을 살려야 하므로 ControllerDesiredRotation 끔.
	const bool bShouldStrafe =
		(bIsLockedOn || bIsGuarding) &&
		!bIsDodging &&
		!bIsMovementLocked;

	MovementComponent->bOrientRotationToMovement = !bShouldStrafe;
	MovementComponent->bUseControllerDesiredRotation = bShouldStrafe;
}

bool UPlayerLocomotionComponent::CanMove() const
{
	if (!StateComponent)
	{
		return false;
	}

	FGameplayTagContainer BlockTags;
	BlockTags.AddTag(CombatTags::State_Combat_Attacking);
	BlockTags.AddTag(CombatTags::State_Combat_Dodging);
	// BlockTags.AddTag(CombatTags::State_Combat_Guarding);
	BlockTags.AddTag(CombatTags::State_Combat_Parry);
	BlockTags.AddTag(CombatTags::State_Movement_Locked);
	BlockTags.AddTag(CombatTags::State_Hit_PostureBroken);
	BlockTags.AddTag(CombatTags::State_Hit_Dead);
	BlockTags.AddTag(CombatTags::State_Hit_Reacting);

	return !StateComponent->HasAnyStateTags(BlockTags);
}

bool UPlayerLocomotionComponent::CanSprint() const
{
	if (!StateComponent || !MovementComponent)
	{
		return false;
	}

	if (MovementComponent->Velocity.IsNearlyZero())
	{
		return false;
	}

	FGameplayTagContainer BlockTags;
	BlockTags.AddTag(CombatTags::State_Combat_Attacking);
	BlockTags.AddTag(CombatTags::State_Combat_Dodging);
	BlockTags.AddTag(CombatTags::State_Combat_Guarding);
	BlockTags.AddTag(CombatTags::State_Combat_Parry);
	BlockTags.AddTag(CombatTags::State_Movement_Locked);
	BlockTags.AddTag(CombatTags::State_Hit_PostureBroken);
	BlockTags.AddTag(CombatTags::State_Hit_Dead);
	BlockTags.AddTag(CombatTags::State_Hit_Reacting);

	return !StateComponent->HasAnyStateTags(BlockTags);
}

bool UPlayerLocomotionComponent::CanDodge() const
{
	if (!StateComponent)
	{
		return false;
	}

	FGameplayTagContainer BlockTags;
	BlockTags.AddTag(CombatTags::State_Combat_Attacking);
	BlockTags.AddTag(CombatTags::State_Combat_Dodging);
	BlockTags.AddTag(CombatTags::State_Combat_Guarding);
	BlockTags.AddTag(CombatTags::State_Combat_Parry);
	BlockTags.AddTag(CombatTags::State_Movement_Locked);
	BlockTags.AddTag(CombatTags::State_Hit_PostureBroken);
	BlockTags.AddTag(CombatTags::State_Hit_Dead);
	BlockTags.AddTag(CombatTags::State_Hit_Reacting);

	return !StateComponent->HasAnyStateTags(BlockTags);
}

void UPlayerLocomotionComponent::DoStartJump(const FInputActionValue& value)
{
	if (!OwnerCharacter) return;
	
	if (StateComponent && StateComponent->HasStateTag(CombatTags::State_Movement_Locked))
	{
		return;	// 이동 불가 시
	}
	
	OwnerCharacter->Jump();
}

void UPlayerLocomotionComponent::DoStopJump(const FInputActionValue& value)
{
	if (!OwnerCharacter) return;
	
	OwnerCharacter->StopJumping();
}

void UPlayerLocomotionComponent::DoMove(const FInputActionValue& Value)
{
	if (!OwnerCharacter || !OwnerCharacter->GetStateTagComponent())
	{
		return;
	}

	UStateTagComponent* StateComp = OwnerCharacter->GetStateTagComponent();
	
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (MovementVector.IsNearlyZero())
	{
		return;
	}

	LastMovementInput = MovementVector;
	
	if (!CanMove())
	{
		return;
	}

	const FRotator Rotation = OwnerCharacter->GetControlRotation();
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	const FVector ForwardDirection =
		FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

	const FVector RightDirection =
		FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	OwnerCharacter->AddMovementInput(
		ForwardDirection,
		MovementVector.X
	);

	OwnerCharacter->AddMovementInput(
		RightDirection,
		MovementVector.Y
	);
	
	// UE_LOG(LogTemp, Warning, TEXT("Move Input Called"));
}

void UPlayerLocomotionComponent::DoStopMove()
{
	LastMovementInput = FVector2D::ZeroVector;
}

void UPlayerLocomotionComponent::DoStartSprint()
{
	if (!OwnerCharacter || !StateComponent || !MovementComponent)
	{
		return;
	}

	if (!CanSprint())
	{
		return;
	}

	StateComponent->AddStateTag(
		CombatTags::State_Movement_Sprinting
	);

	RefreshMovementSettings();
}

void UPlayerLocomotionComponent::DoStopSprint()
{
	if (!StateComponent || !MovementComponent)
	{
		return;
	}

	StateComponent->RemoveStateTag(
		CombatTags::State_Movement_Sprinting
	);

	RefreshMovementSettings();
}

void UPlayerLocomotionComponent::OnSprintDodgePressed(const FInputActionValue& Value)
{
	if (!OwnerCharacter || !GetWorld())
	{
		return;
	}

	bSprintDodgeHeld = true;
	bSprintStartedByHold = false;

	SprintDodgePressedTime = GetWorld()->GetTimeSeconds();

	GetWorld()->GetTimerManager().SetTimer(
		SprintHoldTimerHandle,
		this,
		&UPlayerLocomotionComponent::TryStartSprintByHold,
		SprintHoldThreshold,
		false
	);
}

void UPlayerLocomotionComponent::OnSprintDodgeReleased(const FInputActionValue& Value)
{
	if (!OwnerCharacter || !GetWorld())
	{
		return;
	}

	bSprintDodgeHeld = false;

	GetWorld()->GetTimerManager().ClearTimer(SprintHoldTimerHandle);

	const float HeldTime =
		GetWorld()->GetTimeSeconds() - SprintDodgePressedTime;

	if (bSprintStartedByHold)
	{
		bSprintStartedByHold = false;
		DoStopSprint();
		return;
	}

	if (HeldTime < SprintHoldThreshold)
	{
		RequestDodge();
	}
}

void UPlayerLocomotionComponent::TryStartSprintByHold()
{
	if (!bSprintDodgeHeld)
	{
		return;
	}

	if (!CanSprint())
	{
		return;
	}

	bSprintStartedByHold = true;
	DoStartSprint();
}

void UPlayerLocomotionComponent::RequestDodge()
{
	const bool bIsAttacking =
		StateComponent &&
		StateComponent->HasStateTagExact(
			CombatTags::State_Combat_Attacking
		);

	const bool bCanDodgeFromAttackRecovery =
		bIsAttacking && bDodgeBufferWindowOpen;

	if (CanDodge() || bCanDodgeFromAttackRecovery)
	{
		StartDodge(GetDodgeWorldDirectionFromLastInput());
		return;
	}

	if (!bDodgeBufferWindowOpen)
	{
		return;
	}

	bDodgeBuffered = true;
	BufferedDodgeDirection =
		GetDodgeWorldDirectionFromLastInput();

	if (!GetWorld())
	{
		return;
	}

	GetWorld()->GetTimerManager().ClearTimer(DodgeBufferTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(
		DodgeBufferTimerHandle,
		this,
		&UPlayerLocomotionComponent::ClearDodgeBuffer,
		DodgeBufferDuration,
		false
	);
}

void UPlayerLocomotionComponent::OpenDodgeBufferWindow()
{
	bDodgeBufferWindowOpen = true;
}

void UPlayerLocomotionComponent::CloseDodgeBufferWindow()
{
	bDodgeBufferWindowOpen = false;
	ClearDodgeBuffer();
}

void UPlayerLocomotionComponent::ConsumeDodgeBuffer()
{
	bDodgeBufferWindowOpen = false;

	if (!bDodgeBuffered)
	{
		return;
	}

	bDodgeBuffered = false;

	if (CanDodge())
	{
		StartDodge(BufferedDodgeDirection);
	}
}

void UPlayerLocomotionComponent::ClearDodgeBuffer()
{
	bDodgeBuffered = false;
	BufferedDodgeDirection = FVector::ZeroVector;
}
void UPlayerLocomotionComponent::StartDodge(
	const FVector& DodgeDirection)
{
	if (!OwnerCharacter || !StateComponent || !LocomotionData->DodgeMontage)
	{
		return;
	}

	DoStopSprint();

	const FVector SafeDodgeDirection =
		DodgeDirection.IsNearlyZero()
			? OwnerCharacter->GetActorForwardVector()
			: DodgeDirection.GetSafeNormal2D();

	FRotator DodgeRotation = SafeDodgeDirection.Rotation();
	DodgeRotation.Pitch = 0.0f;
	DodgeRotation.Roll = 0.0f;

	OwnerCharacter->SetActorRotation(DodgeRotation);

	StateComponent->AddStateTag(
		CombatTags::State_Combat_Dodging
	);

	StateComponent->AddStateTag(
		CombatTags::State_Movement_Locked
	);

	RefreshMovementSettings();

	const float Duration =
		OwnerCharacter->PlayAnimMontage(LocomotionData->DodgeMontage);

	if (Duration <= 0.0f)
	{
		EndDodge();
	}
}

void UPlayerLocomotionComponent::EndDodge()
{
	if (!StateComponent)
	{
		return;
	}

	StateComponent->RemoveStateTag(
		CombatTags::State_Combat_Dodging
	);

	StateComponent->RemoveStateTag(
		CombatTags::State_Movement_Locked
	);

	if (CombatComponent)
	{
		DefenseComponent->DisableInvincible();
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(DodgeEndTimerHandle);
	}

	RefreshMovementSettings();
}