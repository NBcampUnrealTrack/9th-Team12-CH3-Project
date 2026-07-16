// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyTransitionComponent.generated.h"

class UNiagaraComponent;
class UNiagaraSystem;
class USkeletalMeshComponent;
class UMaterialInstanceDynamic;
class UMaterialInterface;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FOnTransitionFinished
);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CH3_TEAM12_API UEnemyTransitionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEnemyTransitionComponent();

	void PlayDeathTransition(USkeletalMeshComponent* TargetMesh, UNiagaraSystem* DeathVFX);

	float GetDissolveDuration() {return DissolveDuration;}
protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

public:
	FOnTransitionFinished OnTransitionFinishedDelegate;
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Enemy | Transition")
	FName DissolveParameterName = TEXT("DissolveAmount");

	UPROPERTY(EditDefaultsOnly, Category="Enemy | Transition", meta=(ClampMin=0.01, UIMin=0.01))
	float DissolveDuration = 5.0f;

	UPROPERTY(EditDefaultsOnly, Category="Enemy | Transition", meta=(ClampMin=0.0, UIMin=0.0))
	float MeshHideDelayAfterDissolve = 0.75f;

	UPROPERTY(EditDefaultsOnly, Category="Enemy | Transition")
	TObjectPtr<UMaterialInterface> DissolveFallbackMaterial;

	UPROPERTY()
	TObjectPtr<USkeletalMeshComponent> CachedMesh;

	UPROPERTY()
	TArray<TObjectPtr<UMaterialInstanceDynamic>> DynamicMaterials;

	UPROPERTY()
	TObjectPtr<UNiagaraComponent> SpawnedVFX;

	FTimerHandle MeshHideTimerHandle;

	float ElapsedTime = 0.0f;
	bool bTransitioning = false;

	void CreateDynamicMaterials();
	bool SupportsDissolveParameter(UMaterialInterface* Material) const;
	void SetDissolveAmount(float Amount);
	void FinishTransition();
	void HideMesh();
};
