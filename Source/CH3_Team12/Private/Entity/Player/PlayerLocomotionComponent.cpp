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
	AttackComponent = OwnerCharacter->GetAttackComponent();
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

void UPlayerLocomotionComponent::StartDodge()
{
	if (!OwnerCharacter || !StateComponent || !DodgeData)
	{
		return;
	}
	
	if (AttackComponent && AttackComponent->CanDodgeCancel())
	{
		AttackComponent->CancelAttackForDodge();
	}

	const EDodgeDirection DodgeDirection =
		ShouldUseDirectionalDodge()
			? CalculateDodgeDirectionFromInput(LastMovementInput)
			: EDodgeDirection::Forward;

	const FEvadeMontageData* EvadeData =
		DodgeData->FindEvadeData(DodgeDirection);

	if (!EvadeData || !EvadeData->Montage)
	{
		UE_LOG(LogTemp, Warning, TEXT("StartDodge: EvadeData invalid"));
		return;
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
		TEXT("Dodge Direction: %s / Directional: %s"),
		*UEnum::GetValueAsString(DodgeDirection),
		ShouldUseDirectionalDodge() ? TEXT("true") : TEXT("false")
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

bool UPlayerLocomotionComponent::ShouldUseDirectionalDodge() const
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

	return bIsArmed && bIsLockedOn;
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

void UPlayerLocomotionComponent::Dodge(
	const FInputActionValue& Value)
{
	const bool bCanDodgeCancelAttack =
		AttackComponent &&
		AttackComponent->CanDodgeCancel();

	if (!CanDodge() && !bCanDodgeCancelAttack)
	{
		return;
	}

	StartDodge();
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