#include "Entity/Player/PlayerLocomotionComponent.h"
#include "Entity/Player/StateTagComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"

UPlayerLocomotionComponent::UPlayerLocomotionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}

void UPlayerLocomotionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerCharacter = Cast<APlayerCharacterBase>(GetOwner());
}

void UPlayerLocomotionComponent::DoJump(bool bStartJump) const
{
	if (!OwnerCharacter) return;
	
	if (bStartJump)	// 점프 시작
	{
		UStateTagComponent* StateComp = OwnerCharacter->GetStateTagComponent();
		if (StateComp &&
			StateComp->HasStateTag(FGameplayTag::RequestGameplayTag(TEXT("State.Movement.Locked"))))
		{
			return;	// 이동 불가 시
		}
		OwnerCharacter->Jump();
	}
	else	// 점프 끝
	{
		OwnerCharacter->StopJumping();
	}
}

void UPlayerLocomotionComponent::DoMove(const FVector2D& MovementVector)
{
	if (!OwnerCharacter || !OwnerCharacter->GetStateTagComponent()) return;

	UStateTagComponent* StateComp = OwnerCharacter->GetStateTagComponent();
	
	if (StateComp->HasStateTag(FGameplayTag::RequestGameplayTag(TEXT("State.Movement.Locked"))))
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

// bStateSprint 가 true 일 때 달리기 false 면 걷기
void UPlayerLocomotionComponent::DoSprint(bool bStartSprint)
{
	if (!OwnerCharacter || !OwnerCharacter->GetStateTagComponent()) return;
    
	UStateTagComponent* StateComp = OwnerCharacter->GetStateTagComponent();
	UCharacterMovementComponent* MoveComp = OwnerCharacter->GetCharacterMovement();
	
	if (!MoveComp) return;

	if (bStartSprint)	// 달리기 시작
	{
		if (StateComp->HasStateTag(
			FGameplayTag::RequestGameplayTag(TEXT("State.Movement.Locked"))) || 
			MoveComp->Velocity.IsNearlyZero())
		{
			return;
		}

		StateComp->AddStateTag(
				FGameplayTag::RequestGameplayTag(TEXT("State.Movement.Sprinting")));
        
		MoveComp->MaxWalkSpeed = SprintSpeed;
	}
	else
	{
		if (StateComp->HasStateTag(
			FGameplayTag::RequestGameplayTag(TEXT("State.Movement.Sprinting"))))
		{
			StateComp->RemoveStateTag(
				FGameplayTag::RequestGameplayTag(TEXT("State.Movement.Sprinting")));
		}
        
		MoveComp->MaxWalkSpeed = NormalWalkSpeed;
	}
}


