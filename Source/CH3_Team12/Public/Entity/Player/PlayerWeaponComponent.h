// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerWeaponComponent.generated.h"

class APlayerCharacterBase;
class UPlayerEquipmentComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH3_TEAM12_API UPlayerWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;
	
	UPROPERTY()
	TObjectPtr<UPlayerEquipmentComponent> EquipmentComponent;
	
	void StartWeaponHitCheck();
	void WeaponTrace();
	void EndWeaponHitCheck();
	
	void ProcessHit(const FHitResult& Hit);
	void CacheWeaponTraceLocation();
	
	FVector PreviousBladeStart = FVector::ZeroVector;
	FVector PreviousBladeEnd = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Trace")
	float TraceRadius = 8.0f;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Trace")
	int32 TraceSampleCount = 5;

	UPROPERTY(EditDefaultsOnly, Category="Combat|Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;

	TSet<TWeakObjectPtr<AActor>> HitActors;
	
	bool bWeaponHitCheck = false;
};
