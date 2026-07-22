#include "Entity/Player/FootstepComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/World.h"
#include "Framework/Subsystem/KatanaSoundManagerSubsystem.h"

UFootstepComponent::UFootstepComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFootstepComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<APlayerCharacterBase>(GetOwner());
}

void UFootstepComponent::PlayFootstep(FName FootSocketName)
{
	if (!OwnerCharacter)
	{
		return;
	}

	UCharacterMovementComponent* MovementComponent =
		OwnerCharacter->GetCharacterMovement();

	if (!MovementComponent)
	{
		return;
	}

	const float Now = GetWorld()->GetTimeSeconds();

	if (Now - LastFootstepTime < MinFootstepInterval)
	{
		return;
	}

	LastFootstepTime = Now;

	if (MovementComponent->IsFalling())
	{
		return;
	}

	if (OwnerCharacter->GetVelocity().Size2D() < MinSpeedToPlay)
	{
		return;
	}

	USkeletalMeshComponent* Mesh =
		OwnerCharacter->GetMesh();

	if (!Mesh || !Mesh->DoesSocketExist(FootSocketName))
	{
		return;
	}

	const FVector FootLocation =
		Mesh->GetSocketLocation(FootSocketName);

	const FVector TraceStart =
		FootLocation + FVector(0.0f, 0.0f, 10.0f);

	const FVector TraceEnd =
		FootLocation - FVector(0.0f, 0.0f, TraceDistance);

	FHitResult Hit;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	Params.bReturnPhysicalMaterial = true;

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		Params
	);

	const FVector SoundLocation =
		bHit ? Hit.ImpactPoint : FootLocation;

	if (DefaultFootstepSound)
	{
		UKatanaSoundManagerSubsystem::PlaySoundAtLocation(
			this,
			EAudioType::SFX,
			DefaultFootstepSound,
			SoundLocation,
			VolumeMultiplier
		);
	}

	if (DefaultFootstepEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			DefaultFootstepEffect,
			SoundLocation,
			FRotator::ZeroRotator
		);
	}
}