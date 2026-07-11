// Fill out your copyright notice in the Description page of Project Settings.
#include "Entity/Player/PlayerWeaponComponent.h"

#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerEquipmentComponent.h"
#include "Entity/Weapon/WeaponBase.h"

#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UPlayerWeaponComponent::UPlayerWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UPlayerWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerCharacter = Cast<APlayerCharacterBase>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCombatComponent : OwnerCharacter is nullptr"));
		return;
	}
	
	EquipmentComponent = OwnerCharacter->GetEquipmentComponent();
	if (!EquipmentComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCombatComponent : EquipmentComponent is nullptr"));
	}
}

void UPlayerWeaponComponent::StartWeaponHitCheck()
{
	if (!EquipmentComponent->GetEquippedWeapon())
	{
		return;
	}

	HitActors.Empty();
	CacheWeaponTraceLocation();

	bWeaponHitCheck = true;
}

void UPlayerWeaponComponent::EndWeaponHitCheck()
{
	bWeaponHitCheck = false;

	HitActors.Empty();

	PreviousBladeStart = FVector::ZeroVector;
	PreviousBladeEnd = FVector::ZeroVector;
}

void UPlayerWeaponComponent::CacheWeaponTraceLocation()
{
	if (!EquipmentComponent->GetEquippedWeapon())
	{
		return;
	}

	PreviousBladeStart = EquipmentComponent->GetEquippedWeapon()->GetBladeStartLocation();
	PreviousBladeEnd = EquipmentComponent->GetEquippedWeapon()->GetBladeEndLocation();
}


void UPlayerWeaponComponent::ProcessHit(const FHitResult& Hit)
{
	AActor* HitActor = Hit.GetActor();
	if (!HitActor || !OwnerCharacter)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Hit : %s"), *HitActor->GetName());

	UGameplayStatics::ApplyDamage(
		HitActor,
		25.0f,
		OwnerCharacter->GetController(),
		OwnerCharacter,
		nullptr
	);
}

void UPlayerWeaponComponent::WeaponTrace()
{
	if (!bWeaponHitCheck || !EquipmentComponent->GetEquippedWeapon() || !OwnerCharacter)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector CurrentBladeStart =
		EquipmentComponent->GetEquippedWeapon()->GetBladeStartLocation();

	const FVector CurrentBladeEnd =
		EquipmentComponent->GetEquippedWeapon()->GetBladeEndLocation();

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);
	Params.AddIgnoredActor(EquipmentComponent->GetEquippedWeapon());

	const FCollisionShape CollisionShape =
		FCollisionShape::MakeSphere(TraceRadius);

	const int32 SafeSampleCount = FMath::Max(TraceSampleCount, 2);

	for (int32 Index = 0; Index < SafeSampleCount; ++Index)
	{
		const float Alpha =
			static_cast<float>(Index) /
			static_cast<float>(SafeSampleCount - 1);

		const FVector PreviousPoint =
			FMath::Lerp(PreviousBladeStart, PreviousBladeEnd, Alpha);

		const FVector CurrentPoint =
			FMath::Lerp(CurrentBladeStart, CurrentBladeEnd, Alpha);

		TArray<FHitResult> HitResults;

		const bool bHit = World->SweepMultiByChannel(
			HitResults,
			PreviousPoint,
			CurrentPoint,
			FQuat::Identity,
			TraceChannel,
			CollisionShape,
			Params
		);

		if (!bHit)
		{
			continue;
		}

		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();

			if (!HitActor || HitActor == OwnerCharacter)
			{
				continue;
			}

			if (HitActors.Contains(HitActor))
			{
				continue;
			}

			HitActors.Add(HitActor);
			ProcessHit(Hit);
		}
	}

	PreviousBladeStart = CurrentBladeStart;
	PreviousBladeEnd = CurrentBladeEnd;
}