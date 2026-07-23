#include "Entity/Player/PlayerLocomotionComponent.h"
#include "InputActionValue.h"
#include "GameplayTags/CombatGameplayTags.h"
#include "Entity/Player/StateTagComponent.h"
#include "Entity/Player/PlayerAttackComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerCameraComponent.h"
#include "Entity/Player/PlayerDefenseComponent.h"
#include "Entity/Player/PlayerItemUseComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"

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
	AttackComponent = OwnerCharacter->GetAttackComponent();
	DefenseComponent = OwnerCharacter->GetDefenseComponent();
	ItemUseComponent = OwnerCharacter->GetItemUseComponent();
	
	if (MovementComponent && LocomotionData)
	{
		MovementComponent->bOrientRotationToMovement = true;
		MovementComponent->bUseControllerDesiredRotation = false;
		MovementComponent->RotationRate = FRotator(0.0f, 1000.0f, 0.0f);
		MovementComponent->MaxAcceleration = 2048.0f;
		MovementComponent->GroundFriction = 4.0f;
		MovementComponent->BrakingDecelerationWalking = 200.0f;
		
		MovementComponent->JumpZVelocity = LocomotionData->JumpZVelocity;
		MovementComponent->GravityScale = LocomotionData->GravityScale;
		MovementComponent->AirControl = LocomotionData->AirControl;
		MovementComponent->FallingLateralFriction = 0.5f;
	}
	
	if (OwnerCharacter)
	{
		OwnerCharacter->JumpMaxHoldTime = 0.0f;
		OwnerCharacter->JumpMaxCount = 1;
	}
	
	{
		// Speed
		NormalWalkSpeed = LocomotionData->NormalWalkSpeed;
		WalkSpeed = LocomotionData->WalkSpeed;
		SprintSpeed = LocomotionData->SprintSpeed;
		LockOnWalkSpeed = LocomotionData->LockOnWalkSpeed;
		GuardWalkSpeed = LocomotionData->GuardWalkSpeed;
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

	const bool bIsUsingItem =
		StateComponent->HasStateTagExact(
			CombatTags::State_Action_UsingItem
		);
	// =========================
	// Speed
	// =========================
	if (bIsGuarding)
	{
		MovementComponent->MaxWalkSpeed = GuardWalkSpeed;
	}
	else if (bIsUsingItem)
	{
		MovementComponent->MaxWalkSpeed = 
			NormalWalkSpeed * ItemUseComponent->GetCurrentMoveSpeedMultiplier();
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

	const bool bShouldFreeMoveBySprint =
	bIsSprinting &&
	!bIsMovementLocked;

	const bool bShouldStrafe =
		(bIsLockedOn || bIsGuarding) &&
		!bShouldFreeMoveBySprint &&
		!bIsMovementLocked;

	MovementComponent->bOrientRotationToMovement =
		!bShouldStrafe;

	MovementComponent->bUseControllerDesiredRotation =
		bShouldStrafe;
}

bool UPlayerLocomotionComponent::CanMove() const
{
	if (!StateComponent)
	{
		return false;
	}

	FGameplayTagContainer BlockTags;
	BlockTags.AddTag(CombatTags::State_Combat_Attacking);
	// 여기서는 Dodging을 막지 않는다.
	// BlockTags.AddTag(CombatTags::State_Combat_Dodging);
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

	if (LastMovementInput.IsNearlyZero())
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
	BlockTags.AddTag(CombatTags::State_Action_Equipping);
	BlockTags.AddTag(CombatTags::State_Combat_Guarding);
	BlockTags.AddTag(CombatTags::State_Combat_Parry);
	BlockTags.AddTag(CombatTags::State_Movement_Locked);
	BlockTags.AddTag(CombatTags::State_Hit_PostureBroken);
	BlockTags.AddTag(CombatTags::State_Hit_Dead);
	BlockTags.AddTag(CombatTags::State_Hit_Reacting);

	return !StateComponent->HasAnyStateTags(BlockTags);
}

void UPlayerLocomotionComponent::StartDodge()
{
	if (!OwnerCharacter || !StateComponent || !DodgeData)
	{
		return;
	}

	const bool bIsAttackDodgeCancel =
		AttackComponent &&
		AttackComponent->CanDodgeCancel();

	const bool bUseDirectionalDodge =
		ShouldUseDirectionalDodge(bIsAttackDodgeCancel);

	const EDodgeDirection DodgeDirection =
		bUseDirectionalDodge
			? CalculateDodgeDirectionFromInput(LastMovementInput)
			: EDodgeDirection::Forward;

	const FEvadeMontageData* EvadeData =
		DodgeData->FindEvadeData(DodgeDirection);

	if (!EvadeData || !EvadeData->Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartDodge: EvadeData invalid"));
		return;
	}

	if (bIsAttackDodgeCancel)
	{
		AttackComponent->CancelAttackForDodge();
	}

	if (bUseDirectionalDodge)
	{
		const FRotator DodgeBaseRotation =
			GetDodgeBaseRotation();

		OwnerCharacter->SetActorRotation(DodgeBaseRotation);
	}

	if (StateComponent->HasStateTagExact(
		CombatTags::State_Movement_Sprinting))
	{
		StateComponent->RemoveStateTag(
			CombatTags::State_Movement_Sprinting
		);
	}

	StateComponent->AddStateTag(
		CombatTags::State_Combat_Dodging
	);

	StateComponent->AddStateTag(
		CombatTags::State_Movement_Locked
	);

	RefreshMovementSettings();

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Dodge Direction: %s / Directional: %s / AttackCancel: %s"),
		*UEnum::GetValueAsString(DodgeDirection),
		bUseDirectionalDodge ? TEXT("true") : TEXT("false"),
		bIsAttackDodgeCancel ? TEXT("true") : TEXT("false")
	);

	PlayDodgeMontage(*EvadeData);
}

void UPlayerLocomotionComponent::PlayDodgeMontage(
	const FEvadeMontageData& EvadeData)
{
	if (!OwnerCharacter || !EvadeData.Montage)
	{
		EndDodge();
		return;
	}

	USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh();
	if (!Mesh)
	{
		EndDodge();
		return;
	}

	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	if (!AnimInstance)
	{
		EndDodge();
		return;
	}

	CurrentDodgeMontage = EvadeData.Montage;

	const float Duration =
		AnimInstance->Montage_Play(
			EvadeData.Montage,
			EvadeData.PlayRate
		);

	if (Duration <= 0.0f)
	{
		EndDodge();
		return;
	}

	if (EvadeData.SectionName != NAME_None)
	{
		AnimInstance->Montage_JumpToSection(
			EvadeData.SectionName,
			EvadeData.Montage
		);
	}

	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(
		this,
		&UPlayerLocomotionComponent::OnDodgeMontageEnded
	);

	AnimInstance->Montage_SetEndDelegate(
		EndDelegate,
		EvadeData.Montage
	);
}

void UPlayerLocomotionComponent::OnDodgeMontageEnded(
	UAnimMontage* Montage,
	bool bInterrupted)
{
	if (Montage != CurrentDodgeMontage)
	{
		return;
	}

	EndDodge();
}

EDodgeDirection UPlayerLocomotionComponent::CalculateDodgeDirectionFromInput(
	const FVector2D& InputValue) const
{
	if (!DodgeData)
	{
		return EDodgeDirection::Forward;
	}

	if (InputValue.SizeSquared() <
		FMath::Square(DodgeData->DirectionDeadZone))
	{
		return DodgeData->NoInputDirection;
	}

	const FVector2D NormalizedInput =
		InputValue.GetSafeNormal();

	const float ForwardValue = NormalizedInput.X;
	const float RightValue = NormalizedInput.Y;

	const float AngleDegrees =
		FMath::RadiansToDegrees(
			FMath::Atan2(RightValue, ForwardValue)
		);

	if (AngleDegrees >= -22.5f && AngleDegrees < 22.5f)
	{
		return EDodgeDirection::Forward;
	}

	if (AngleDegrees >= 22.5f && AngleDegrees < 67.5f)
	{
		return EDodgeDirection::ForwardRight;
	}

	if (AngleDegrees >= 67.5f && AngleDegrees < 112.5f)
	{
		return EDodgeDirection::Right;
	}

	if (AngleDegrees >= 112.5f && AngleDegrees < 157.5f)
	{
		return EDodgeDirection::BackwardRight;
	}

	if (AngleDegrees >= 157.5f || AngleDegrees < -157.5f)
	{
		return EDodgeDirection::Backward;
	}

	if (AngleDegrees >= -157.5f && AngleDegrees < -112.5f)
	{
		return EDodgeDirection::BackwardLeft;
	}

	if (AngleDegrees >= -112.5f && AngleDegrees < -67.5f)
	{
		return EDodgeDirection::Left;
	}

	if (AngleDegrees >= -67.5f && AngleDegrees < -22.5f)
	{
		return EDodgeDirection::ForwardLeft;
	}

	return EDodgeDirection::Forward;
}

bool UPlayerLocomotionComponent::ShouldUseDirectionalDodge(
	bool bIsAttackDodgeCancel) const
{
	if (!StateComponent)
	{
		return false;
	}

	const bool bIsArmed =
		StateComponent->HasStateTagExact(
			CombatTags::State_Combat_Armed
		);

	const bool bIsLockedOn =
		StateComponent->HasStateTagExact(
			CombatTags::State_Movement_LockOn
		);

	// 무기 안 들었으면 기본적으로 방향 회피 안 씀.
	if (!bIsArmed)
	{
		return false;
	}

	// 락온 중이면 기존처럼 8방향.
	if (bIsLockedOn)
	{
		return true;
	}

	// 공격 후 Dodge Cancel이면 비락온이어도 8방향 허용.
	if (bIsAttackDodgeCancel)
	{
		return true;
	}

	return false;
}

FRotator UPlayerLocomotionComponent::GetDodgeBaseRotation() const
{
	if (!OwnerCharacter)
	{
		return FRotator::ZeroRotator;
	}

	const bool bIsLockedOn =
		StateComponent &&
		StateComponent->HasStateTagExact(
			CombatTags::State_Movement_LockOn
		);

	if (bIsLockedOn)
	{
		if (UPlayerCameraComponent* PlayerCameraComponent =
			OwnerCharacter->GetPlayerCameraComponent())
		{
			if (AActor* LockOnTarget =
				PlayerCameraComponent->GetCurrentLockOnTarget())
			{
				FVector ToTarget =
					LockOnTarget->GetActorLocation() -
					OwnerCharacter->GetActorLocation();

				ToTarget.Z = 0.0f;

				if (!ToTarget.IsNearlyZero())
				{
					return ToTarget.Rotation();
				}
			}
		}
	}

	const FRotator ControlRotation =
		OwnerCharacter->GetControlRotation();

	return FRotator(0.0f, ControlRotation.Yaw, 0.0f);
}

bool UPlayerLocomotionComponent::TryStartDodge()
{
	const bool bCanDodgeCancelAttack =
		AttackComponent &&
		AttackComponent->CanDodgeCancel();

	if (!CanDodge() && !bCanDodgeCancelAttack)
	{
		return false;
	}

	StartDodge();
	return true;
}

void UPlayerLocomotionComponent::TryStartSprintAfterDodge()
{
	if (!bDodgeSprintHeld)
	{
		return;
	}

	if (!bWantsSprintAfterDodge)
	{
		return;
	}

	bWantsSprintAfterDodge = false;

	if (CanSprint())
	{
		DoStartSprint();
	}
}

void UPlayerLocomotionComponent::BufferDodgeInput()
{
	if (!GetWorld())
	{
		return;
	}

	bBufferedDodgeInput = true;
	BufferedDodgeInputTime = GetWorld()->GetTimeSeconds();
}

bool UPlayerLocomotionComponent::HasValidBufferedDodgeInput() const
{
	if (!bBufferedDodgeInput || !GetWorld())
	{
		return false;
	}

	const float Now = GetWorld()->GetTimeSeconds();

	return Now - BufferedDodgeInputTime <= DodgeInputBufferDuration;
}

void UPlayerLocomotionComponent::ClearBufferedDodgeInput()
{
	bBufferedDodgeInput = false;
	BufferedDodgeInputTime = 0.0f;
}

bool UPlayerLocomotionComponent::TryConsumeBufferedDodge()
{
	if (!HasValidBufferedDodgeInput())
	{
		ClearBufferedDodgeInput();
		return false;
	}

	ClearBufferedDodgeInput();

	return TryStartDodge();
}

void UPlayerLocomotionComponent::DoStartJump(
	const FInputActionValue& Value)
{
	if (!CanStartJump())
	{
		return;
	}

	if (StateComponent)
	{
		StateComponent->AddStateTag(
			CombatTags::State_Movement_JumpStarting
		);
	}

	RefreshMovementSettings();
}

void UPlayerLocomotionComponent::DoStopJump(
	const FInputActionValue& Value)
{
	if (!OwnerCharacter)
	{
		return;
	}

	OwnerCharacter->StopJumping();
}

bool UPlayerLocomotionComponent::CanStartJump() const
{
	if (!OwnerCharacter || !StateComponent || !MovementComponent)
	{
		return false;
	}

	if (!OwnerCharacter->CanJump())
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

void UPlayerLocomotionComponent::CommitJump()
{
	if (!OwnerCharacter || !StateComponent || !MovementComponent)
	{
		return;
	}

	if (!StateComponent->HasStateTagExact(
		CombatTags::State_Movement_JumpStarting))
	{
		return;
	}

	StateComponent->RemoveStateTag(
		CombatTags::State_Movement_JumpStarting
	);

	// 공중 진입 순간 Sprint 태그는 제거해도 됨.
	// 기존 속도는 CharacterMovement Velocity에 남아 있음.
	StateComponent->RemoveStateTag(
		CombatTags::State_Movement_Sprinting
	);

	OwnerCharacter->Jump();

	RefreshMovementSettings();
}

void UPlayerLocomotionComponent::DoMove(
	const FInputActionValue& Value)
{
	if (!OwnerCharacter || !StateComponent)
	{
		return;
	}

	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (MovementVector.IsNearlyZero())
	{
		return;
	}

	LastMovementInput = MovementVector;

	const bool bCanCancelAttackWithMovement =
		AttackComponent &&
		AttackComponent->CanMoveCancel();

	if (bCanCancelAttackWithMovement &&
		HasValidBufferedDodgeInput())
	{
		return;
	}

	if (!CanMove())
	{
		if (!bCanCancelAttackWithMovement)
		{
			return;
		}

		AttackComponent->CancelAttackForMovement();

		if (!CanMove())
		{
			return;
		}
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

void UPlayerLocomotionComponent::Dodge(
	const FInputActionValue& Value)
{
	if (TryStartDodge())
	{
		return;
	}

	// 공격 중이면 Dodge 입력을 짧게 저장한다.
	if (StateComponent &&
		StateComponent->HasStateTagExact(CombatTags::State_Combat_Attacking))
	{
		BufferDodgeInput();
	}
}

void UPlayerLocomotionComponent::EndDodge()
{
	if (!StateComponent)
	{
		return;
	}

	if (!StateComponent->HasStateTagExact(
		CombatTags::State_Combat_Dodging))
	{
		return;
	}

	const bool bIsDead =
		StateComponent->HasStateTagExact(
			CombatTags::State_Hit_Dead
		);

	CurrentDodgeMontage = nullptr;

	StateComponent->RemoveStateTag(
		CombatTags::State_Combat_Dodging
	);

	if (!bIsDead)
	{
		StateComponent->RemoveStateTag(
			CombatTags::State_Movement_Locked
		);
	}

	if (DefenseComponent)
	{
		DefenseComponent->DisableInvincible();
	}

	RefreshMovementSettings();

	TryStartSprintAfterDodge();
}

void UPlayerLocomotionComponent::OpenDodgeMove()
{
	if (!StateComponent)
	{
		return;
	}

	if (!StateComponent->HasStateTagExact(
		CombatTags::State_Combat_Dodging))
	{
		return;
	}

	StateComponent->RemoveStateTag(
		CombatTags::State_Movement_Locked
	);

	RefreshMovementSettings();
}

void UPlayerLocomotionComponent::OnDodgeSprintPressed(
	const FInputActionValue& Value)
{
	if (!OwnerCharacter || !StateComponent)
	{
		return;
	}

	if (bDodgeSprintHeld)
	{
		return;
	}

	bDodgeSprintHeld = true;

	if (TryStartDodge())
	{
		bWantsSprintAfterDodge = true;
		return;
	}

	if (StateComponent->HasStateTagExact(
		CombatTags::State_Combat_Attacking))
	{
		BufferDodgeInput();
	}
}

void UPlayerLocomotionComponent::OnDodgeSprintReleased(
	const FInputActionValue& Value)
{
	if (!bDodgeSprintHeld)
	{
		return;
	}

	bDodgeSprintHeld = false;
	bWantsSprintAfterDodge = false;

	if (StateComponent &&
		StateComponent->HasStateTagExact(
			CombatTags::State_Movement_Sprinting))
	{
		DoStopSprint();
	}
}

void UPlayerLocomotionComponent::CancelDodgeForPostureBreak()
{
	if (!StateComponent)
	{
		return;
	}

	CurrentDodgeMontage = nullptr;

	StateComponent->RemoveStateTag(
		CombatTags::State_Combat_Dodging
	);

	StateComponent->RemoveStateTag(
		CombatTags::State_Movement_Sprinting
	);

	if (DefenseComponent)
	{
		DefenseComponent->DisableInvincible();
	}

	RefreshMovementSettings();
}

void UPlayerLocomotionComponent::CancelDodgeForAttack()
{
	if (!OwnerCharacter || !StateComponent)
	{
		return;
	}

	if (USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh())
	{
		if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
		{
			if (CurrentDodgeMontage)
			{
				AnimInstance->Montage_Stop(
					0.05f,
					CurrentDodgeMontage
				);
			}
		}
	}

	StateComponent->RemoveStateTag(
		CombatTags::State_Combat_Dodging
	);

	StateComponent->RemoveStateTag(
		CombatTags::State_Combat_Invincible
	);

	StateComponent->RemoveStateTag(
		CombatTags::State_Movement_Locked
	);

	RefreshMovementSettings();
}