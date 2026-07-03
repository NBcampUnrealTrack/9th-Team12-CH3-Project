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
	StateComp = OwnerCharacter->GetStateTagComponent();
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

void UPlayerLocomotionComponent::DoStartJump(const FInputActionValue& value)
{
	if (!OwnerCharacter) return;
	
	if (StateComp && StateComp->HasStateTag(CombatTags::State_Movement_Locked))
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

void UPlayerLocomotionComponent::DoMove(const FInputActionValue& value)
{
	const FVector2D MovementVector = value.Get<FVector2D>();
	
	if (!OwnerCharacter || !StateComp) return;
	
	if (StateComp && StateComp->HasStateTag(CombatTags::State_Movement_Locked))
	{
		return;
	}
	
	const FRotator Rotation = OwnerCharacter->GetControlRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	OwnerCharacter->AddMovementInput(ForwardDirection, MovementVector.X);
	
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	OwnerCharacter->AddMovementInput(RightDirection, MovementVector.Y);
}

void UPlayerLocomotionComponent::DoStartSprint(const FInputActionValue& value)
{
	if (!OwnerCharacter || !StateComp) return;
	
	if (!MovementComponent) return;

	if (StateComp->HasStateTag(CombatTags::State_Movement_Locked) || 
		MovementComponent->Velocity.IsNearlyZero())
	{
		return;
	}

	StateComp->AddStateTag(CombatTags::State_Movement_Sprinting);
        
	MovementComponent->MaxWalkSpeed = SprintSpeed;
}

void UPlayerLocomotionComponent::DoStopSprint(const FInputActionValue& value)
{
	if (!OwnerCharacter || !StateComp) return;
	
	if (!MovementComponent) return;
	
	if (StateComp->HasStateTag(CombatTags::State_Movement_Sprinting))
	{
		StateComp->RemoveStateTag(CombatTags::State_Movement_Sprinting);
	}
        
	MovementComponent->MaxWalkSpeed = NormalWalkSpeed;
}