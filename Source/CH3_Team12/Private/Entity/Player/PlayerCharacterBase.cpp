// Fill out your copyright notice in the Description page of Project Settings.

#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/StateTagComponent.h"
#include "EnhancedInputComponent.h"
#include "Entity/Player/PlayerControllerBase.h"

// Sets default values
APlayerCharacterBase::APlayerCharacterBase()
{

	PrimaryActorTick.bCanEverTick = true;

	StateTagComponent = CreateDefaultSubobject<UStateTagComponent>(TEXT("StateComponent"));
}

UStateTagComponent* APlayerCharacterBase::GetStateTagComponent() const
{
	return StateTagComponent;
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
void APlayerCharacterBase::SetupPlayerInputComponent(
	UInputComponent* PlayerInputComponent
)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent =
		Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (!EnhancedInputComponent)
	{
		return;
	}

	APlayerControllerBase* PlayerControllerBase = Cast<APlayerControllerBase>(GetController());
	if (!PlayerControllerBase) return;

}

