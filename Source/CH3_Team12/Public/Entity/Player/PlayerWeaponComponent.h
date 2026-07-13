#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerWeaponComponent.generated.h"

struct FCollisionShape;
struct FAttackHitData;
class UPlayerAttackComponent;
class APlayerCharacterBase;
class UPlayerEquipmentComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH3_TEAM12_API UPlayerWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerWeaponComponent();

protected:
	virtual void BeginPlay() override;

public:
	void StartWeaponHitCheck(int32 HitIndex);
	void WeaponTrace();
	void EndWeaponHitCheck();
	
	void ProcessHit(const FHitResult& Hit);
	void CacheWeaponTraceLocation();
	
	FVector PreviousBladeStart = FVector::ZeroVector;
	FVector PreviousBladeEnd = FVector::ZeroVector;

	TSet<TWeakObjectPtr<AActor>> HitActors;
	
	bool bWeaponHitCheck = false;
	
private:
	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;
	
	UPROPERTY()
	TObjectPtr<UPlayerEquipmentComponent> EquipmentComponent;
	
	UPROPERTY()
	TObjectPtr<UPlayerAttackComponent> AttackComponent;
	
	void ExecuteWeaponTrace();
	void ExecuteSphereTrace();
	void ExecuteCapsuleTrace();
	void ExecuteBoxTrace();
	
	void ExecuteSweep(
	const FVector& Start,
	const FVector& End,
	const FCollisionShape& Shape);
	
	const FAttackHitData* CurrentHit = nullptr;
};
