#pragma once

#include "CoreMinimal.h"
#include "EnemyCharacterBase.h"
#include "DamageDummy.generated.h"

class UBoxComponent;
class USphereComponent;

UCLASS()
class CH3_TEAM12_API ADamageDummy : public AEnemyCharacterBase
{
	GENERATED_BODY()

public:
	ADamageDummy();

protected:
	virtual void BeginPlay() override;

private:
	void PrepareAttack();
	void Attack();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> AttackSphere;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> DummyMesh;
	
	UPROPERTY(EditAnywhere, Category="Attack")
	float AttackInterval = 3.0f;

	UPROPERTY(EditAnywhere, Category="Attack")
	float WarningTime = 0.5f;

	UPROPERTY(EditAnywhere, Category="Attack")
	float HealthDamage = 5.0f;

	UPROPERTY(EditAnywhere, Category="Attack")
	float PostureDamage = 10.0f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> NormalMaterial;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> WarningMaterial;
	
	FTimerHandle AttackTimerHandle;
	FTimerHandle WarningTimerHandle;
};