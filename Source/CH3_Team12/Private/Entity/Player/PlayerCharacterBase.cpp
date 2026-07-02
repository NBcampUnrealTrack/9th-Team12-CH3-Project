// Fill out your copyright notice in the Description page of Project Settings.

#include "Entity/Player/PlayerCharacterBase.h"

#include "EnhancedInputComponent.h"
#include "Entity/Player/PlayerControllerBase.h"
#include "Entity/Player/StateTagComponent.h"
#include "Entity/Player/PlayerLocomotionComponent.h"
#include "Entity/Player/PlayerAttributeComponent.h"

// Sets default values
APlayerCharacterBase::APlayerCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	StateTagComponent = CreateDefaultSubobject<UStateTagComponent>(TEXT("StateComponent"));
	LocomotionComponent = CreateDefaultSubobject<UPlayerLocomotionComponent>(TEXT("LocomotionComponent"));
	AttributeComponent = CreateDefaultSubobject<UPlayerAttributeComponent>(TEXT("AttributeComponent"));
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

	// if (UInputAction* MoveAction = PlayerControllerBase->GetMoveAction())
	// {
	// 	EnhancedInputComponent->BindAction(
	// 		MoveAction,
	// 		ETriggerEvent::Triggered,
	// 		LocomotionComponent,
	// 		&UPlayerLocomotionComponent::DoMove
	// 	);
	// }
}

