#include "Entity/Player/PlayerWeaponComponent.h"
#include "Entity/Player/PlayerAttackComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerEquipmentComponent.h"
#include "Entity/Weapon/WeaponBase.h"
#include "Entity/Enemy/EnemyCharacterBase.h"
#include "Entity/Enemy/Component/EnemyDefenseComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

UPlayerWeaponComponent::UPlayerWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPlayerWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerCharacter = Cast<APlayerCharacterBase>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT(
			"PlayerWeaponComponent : OwnerCharacter is nullptr"));
		return;
	}
	
	EquipmentComponent = OwnerCharacter->GetEquipmentComponent();
	if (!EquipmentComponent)
	{
		UE_LOG(LogTemp, Error, TEXT(
			"PlayerWeaponComponent : EquipmentComponent is nullptr"));
		return;
	}
	
	AttackComponent = OwnerCharacter->GetAttackComponent();
	if (!AttackComponent)
	{
		UE_LOG(LogTemp, Error, TEXT(
			"PlayerWeaponComponent : AttackComponent is nullptr"));
		return;
	}
}

void UPlayerWeaponComponent::StartWeaponHitCheck(int32 HitIndex)
{
	CurrentHit =
		AttackComponent->GetCurrentHit(HitIndex);

	if (!CurrentHit)
	{
		return;
	}

	HitActors.Empty();

	if (CurrentHit->TraceType ==
		EAttackTraceType::Weapon)
	{
		CacheWeaponTraceLocation();
	}

	bWeaponHitCheck = true;
}

void UPlayerWeaponComponent::EndWeaponHitCheck()
{
	bWeaponHitCheck = false;
	CurrentHit = nullptr;
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

void UPlayerWeaponComponent::ExecuteWeaponTrace()
{
	AWeaponBase* Weapon =
		EquipmentComponent->GetEquippedWeapon();
	
	if (!Weapon)
	{
		return;
	}

	const FVector CurrentBladeStart =
		Weapon->GetBladeStartLocation();

	const FVector CurrentBladeEnd =
		Weapon->GetBladeEndLocation();

	const int32 SampleCount =
		FMath::Max(CurrentHit->TraceSampleCount, 2);

	const FCollisionShape Shape =
		FCollisionShape::MakeSphere(
			CurrentHit->TraceRadius);
	
	for (int32 i = 0; i < SampleCount; ++i)
	{
		const float Alpha =
			(float)i / (SampleCount - 1);

		const FVector Prev =
			FMath::Lerp(
				PreviousBladeStart,
				PreviousBladeEnd,
				Alpha);

		const FVector Curr =
			FMath::Lerp(
				CurrentBladeStart,
				CurrentBladeEnd,
				Alpha);

		ExecuteSweep(
			Prev,
			Curr,
			Shape);
	}

	PreviousBladeStart = CurrentBladeStart;
	PreviousBladeEnd = CurrentBladeEnd;
}

void UPlayerWeaponComponent::ExecuteSphereTrace()
{
	AWeaponBase* Weapon =
		EquipmentComponent->GetEquippedWeapon();

	if (!Weapon)
	{
		return;
	}

	const FVector Center =
		Weapon->GetActorLocation();

	ExecuteSweep(
		Center,
		Center,
		FCollisionShape::MakeSphere(
			CurrentHit->TraceRadius));
}

void UPlayerWeaponComponent::ExecuteCapsuleTrace()
{
	AWeaponBase* Weapon =
		EquipmentComponent->GetEquippedWeapon();

	if (!Weapon)
	{
		return;
	}

	const FVector Center =
		Weapon->GetActorLocation();

	ExecuteSweep(
		Center,
		Center,
		FCollisionShape::MakeCapsule(
			CurrentHit->TraceRadius,
			CurrentHit->CapsuleHalfHeight));
}

void UPlayerWeaponComponent::ExecuteBoxTrace()
{
	AWeaponBase* Weapon =
		EquipmentComponent->GetEquippedWeapon();

	if (!Weapon)
	{
		return;
	}

	const FVector Center =
		Weapon->GetActorLocation();

	ExecuteSweep(
		Center,
		Center,
		FCollisionShape::MakeBox(
			CurrentHit->BoxExtent));
}

void UPlayerWeaponComponent::ExecuteSweep(
	const FVector& Start,
	const FVector& End,
	const FCollisionShape& Shape)
{
	if (!CurrentHit)
	{
		return;
	}

	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	AWeaponBase* Weapon =
		EquipmentComponent->GetEquippedWeapon();

	FCollisionQueryParams Params;

	Params.AddIgnoredActor(OwnerCharacter);

	if (Weapon)
	{
		Params.AddIgnoredActor(Weapon);
	}

	TArray<FHitResult> HitResults;

	const bool bHit =
		World->SweepMultiByChannel(
			HitResults,
			Start,
			End,
			FQuat::Identity,
			CurrentHit->TraceChannel,
			Shape,
			Params);

	if (!bHit)
	{
		return;
	}

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();

		if (!HitActor)
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

void UPlayerWeaponComponent::ProcessHit(const FHitResult& Hit)
{
	AActor* HitActor = Hit.GetActor();
	if (!HitActor || 
		!OwnerCharacter
		)
	{
		return;
	}
	
	if (!CurrentHit)
	{
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Hit : %s"), *HitActor->GetName());

	if (AEnemyCharacterBase* EnemyCharacter = Cast<AEnemyCharacterBase>(HitActor))
	{
		if (UEnemyDefenseComponent* EnemyDefenseComponent =
			EnemyCharacter->GetEnemyDefenseComponent())
		{
			FIncomingAttackContext Context;
			Context.Attacker = OwnerCharacter;
			Context.Hit = Hit;
			Context.AttackInfo.Damage = CurrentHit->Damage;
			Context.AttackInfo.PostureDamage = CurrentHit->PostureDamage;
			Context.AttackInfo.bCanBeGuarded = true;
			Context.AttackInfo.bCanBeParried = true;

			FVector AttackDirection =
				EnemyCharacter->GetActorLocation()
				- OwnerCharacter->GetActorLocation();

			AttackDirection.Z = 0.0f;
			Context.AttackWorldDirection =
				AttackDirection.GetSafeNormal();

			EnemyDefenseComponent->ResolveIncomingAttack(Context);
			return;
		}
	}
	else
	{
		UGameplayStatics::ApplyDamage(
		HitActor,
		CurrentHit->Damage,
OwnerCharacter->GetController(),
OwnerCharacter,
nullptr);
	}
}

void UPlayerWeaponComponent::WeaponTrace()
{
	if (!bWeaponHitCheck || !CurrentHit)
	{
		return;
	}

	switch (CurrentHit->TraceType)
	{
	case EAttackTraceType::Weapon:
		ExecuteWeaponTrace();
		break;
	case EAttackTraceType::Sphere:
		ExecuteSphereTrace();
		break;
	case EAttackTraceType::Capsule:
		ExecuteCapsuleTrace();
		break;
	case EAttackTraceType::Box:
		ExecuteBoxTrace();
		break;
	default:
		break;
	}
}
