// Fill out your copyright notice in the Description page of Project Settings.

#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/StateTagComponent.h"

// Sets default values
APlayerCharacterBase::APlayerCharacterBase()
{

	PrimaryActorTick.bCanEverTick = true;

	StateComponent = CreateDefaultSubobject<UStateTagComponent>(
		TEXT("StateComponent")
	);
}

UStateTagComponent* APlayerCharacterBase::GetStateComponent() const
{
	return StateComponent;
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

}

