// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/MeshTransitionComponent.h"

// Sets default values for this component's properties
UMeshTransitionComponent::UMeshTransitionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMeshTransitionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
}


// Called every frame
void UMeshTransitionComponent::TickComponent(float DeltaTime, ELevelTick TickType
                                             , FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UMeshTransitionComponent::UpdateDissolveEffect(float DeltaTime)
{
	for (FDissolveParams& Params : DissolveParamsArray)
	{
		if (!Params.bDissolving)
		{
			Params.DissolveTime += DeltaTime;

			if (Params.DissolveTime >= Params.Delay)
			{
				Params.bDissolving = true;
				Params.DissolveTime = 0.f;
			}
			else
			{
				continue;
			}
		}
		Params.DissolveTime += DeltaTime;

		float DissolveAlpha = Params.DissolveCurve->GetFloatValue(Params.DissolveTime / Params.Duration);
	}
}
