#include "Entity/Weapon/WeaponBase.h"

#include "Components/StaticMeshComponent.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetGenerateOverlapEvents(false);
}

FVector AWeaponBase::GetBladeStartLocation() const
{
	if (!WeaponMesh)
	{
		return GetActorLocation();
	}

	return WeaponMesh->GetSocketLocation(BladeStartSocketName);
}

FVector AWeaponBase::GetBladeEndLocation() const
{
	if (!WeaponMesh)
	{
		return GetActorLocation();
	}

	return WeaponMesh->GetSocketLocation(BladeEndSocketName);
}