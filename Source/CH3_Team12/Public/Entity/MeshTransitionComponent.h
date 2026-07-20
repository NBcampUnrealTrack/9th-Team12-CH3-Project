// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Components/ActorComponent.h"
#include "Curves/CurveFloat.h"
#include "MeshTransitionComponent.generated.h"


class UMeshComponent;

USTRUCT(BlueprintType)
struct FDissolveParams
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDissolveAllMaterials;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "!bDissolveAllMaterials"))
	TArray<int32> MaterialSectionsToDissolve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Delay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Duration = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* DissolveCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* DissolveVFX;

	UPROPERTY()
	bool bDissolving = false;

	UPROPERTY()
	float DissolveTime = 0.f;
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CH3_TEAM12_API UMeshTransitionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMeshTransitionComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType
	                           , FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Transition")
	TArray<FDissolveParams> DissolveParamsArray;

private:
	UPROPERTY()
	UMeshComponent* MeshComponent;

	void UpdateDissolveEffect(float DeltaTime);
};
