// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class UStaticMeshComponent;

UCLASS()
class CH3_TEAM12_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

public:
	UStaticMeshComponent* GetWeaponMesh() const { return WeaponMesh; }

	FVector GetBladeStartLocation() const;
	FVector GetBladeEndLocation() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStaticMeshComponent> WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Socket")
	FName BladeStartSocketName = TEXT("BladeStart");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon|Socket")
	FName BladeEndSocketName = TEXT("BladeEnd");
};
