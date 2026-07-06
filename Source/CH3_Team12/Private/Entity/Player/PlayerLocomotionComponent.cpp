#include "Entity/Player/PlayerLocomotionComponent.h"
#include "InputActionValue.h"
#include "GameplayTags/CombatGameplayTags.h"
#include "Entity/Player/StateTagComponent.h"
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
	StateComponent = OwnerCharacter->GetStateTagComponent();
	MovementComponent = OwnerCharacter->GetCharacterMovement();
	
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

void UPlayerLocomotionComponent::DoStartSprint(const FInputActionValue& value)
{
	if (!OwnerCharacter || !StateComponent) return;
	
	if (!MovementComponent) return;

	if (StateComponent->HasStateTag(CombatTags::State_Movement_Locked) || 
		MovementComponent->Velocity.IsNearlyZero())
	{
		return;
	}

	StateComponent->AddStateTag(CombatTags::State_Movement_Sprinting);
        
	MovementComponent->MaxWalkSpeed = SprintSpeed;
	
}

void UPlayerLocomotionComponent::DoStopSprint(const FInputActionValue& value)
{
	if (!OwnerCharacter || !StateComponent) return;
	
	if (!MovementComponent) return;
	
	if (StateComponent->HasStateTag(CombatTags::State_Movement_Sprinting))
	{
		StateComponent->RemoveStateTag(CombatTags::State_Movement_Sprinting);
	}
        
	MovementComponent->MaxWalkSpeed = NormalWalkSpeed;
}