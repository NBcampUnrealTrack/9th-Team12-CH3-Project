#include "Entity/Player/PlayerLocomotionComponent.h"
#include "InputActionValue.h"
#include "GameplayTags/CombatGameplayTags.h"
#include "Entity/Player/StateTagComponent.h"
#include "Entity/Player/PlayerCombatComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
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

	if (MovementComponent)
	{
		MovementComponent->MaxWalkSpeed = NormalWalkSpeed;
	}
}

void UPlayerLocomotionComponent::Look(const FInputActionValue& value)
{
	if (!OwnerCharacter) return;
	
	const FVector2D LookInput = value.Get<FVector2D>();

	OwnerCharacter->AddControllerYawInput(LookInput.X);
	OwnerCharacter->AddControllerPitchInput(LookInput.Y);
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

	const FRotator ControlRotation =
		OwnerCharacter->GetControlRotation();

	const FRotator YawRotation(
		0.0f,
		ControlRotation.Yaw,
		0.0f
	);

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
	BlockTags.AddTag(CombatTags::State_Movement_Locked);
	BlockTags.AddTag(CombatTags::State_Combat_Attacking);
	BlockTags.AddTag(CombatTags::State_Combat_Dodging);
	BlockTags.AddTag(CombatTags::State_Combat_Guarding);

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
	BlockTags.AddTag(CombatTags::State_Hit_PostureBroken);
	BlockTags.AddTag(CombatTags::State_Hit_Dead);

	return !StateComponent->HasAnyStateTags(BlockTags);
}

void UPlayerLocomotionComponent::DoStopMove()
{
	LastMovementInput = FVector2D::ZeroVector;
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

	if (StateComp->HasStateTagExact(CombatTags::State_Movement_Locked))
	{
		return;
	}

	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (MovementVector.IsNearlyZero())
	{
		return;
	}

	LastMovementInput = MovementVector;

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

	StateComponent->AddStateTag(CombatTags::State_Movement_Sprinting);
	MovementComponent->MaxWalkSpeed = SprintSpeed;
}

void UPlayerLocomotionComponent::DoStopSprint()
{
	if (!StateComponent || !MovementComponent)
	{
		return;
	}

	StateComponent->RemoveStateTag(CombatTags::State_Movement_Sprinting);
	MovementComponent->MaxWalkSpeed = NormalWalkSpeed;
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
	if (CanDodge())
	{
		StartDodge(GetDodgeWorldDirectionFromLastInput());
		return;
	}

	if (!bDodgeBufferWindowOpen)
	{
		return;
	}

	bDodgeBuffered = true;
	BufferedDodgeDirection = GetDodgeWorldDirectionFromLastInput();

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

void UPlayerLocomotionComponent::StartDodge(const FVector& DodgeDirection)
{
	if (!OwnerCharacter || !StateComponent || !DodgeMontage)
	{
		return;
	}

	DoStopSprint();

	if (!DodgeDirection.IsNearlyZero())
	{
		FRotator DodgeRotation = DodgeDirection.Rotation();
		DodgeRotation.Pitch = 0.0f;
		DodgeRotation.Roll = 0.0f;

		OwnerCharacter->SetActorRotation(DodgeRotation);
	}

	StateComponent->AddStateTag(CombatTags::State_Combat_Dodging);
	StateComponent->AddStateTag(CombatTags::State_Movement_Locked);

	const float Duration =
		OwnerCharacter->PlayAnimMontage(DodgeMontage);

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

	StateComponent->RemoveStateTag(CombatTags::State_Combat_Dodging);
	StateComponent->RemoveStateTag(CombatTags::State_Movement_Locked);

	if (CombatComponent)
	{
		CombatComponent->DisableInvincible();
	}
}