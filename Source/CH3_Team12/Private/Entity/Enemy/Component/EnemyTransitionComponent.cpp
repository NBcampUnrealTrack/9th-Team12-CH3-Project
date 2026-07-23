// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/Component/EnemyTransitionComponent.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SkeletalMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialParameters.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

UEnemyTransitionComponent::UEnemyTransitionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	SetAutoActivate(true);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> DefaultDissolveFallbackMaterial(
		TEXT("/Game/Shader/EnemyShading/MI_Enemy_Armor.MI_Enemy_Armor"));
	if (DefaultDissolveFallbackMaterial.Succeeded())
	{
		DissolveFallbackMaterial = DefaultDissolveFallbackMaterial.Object;
	}
}

void UEnemyTransitionComponent::PlayDeathTransition(USkeletalMeshComponent* TargetMesh, UNiagaraSystem* DeathVFX)
{
	if (!TargetMesh || bTransitioning)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyTransition Failed / TargetMesh: %s / bTransitioning: %d"),
			*GetNameSafe(TargetMesh),
			bTransitioning);
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemyTransition Failed / World is null"));
		return;
	}

	Activate(true);
	CachedMesh = TargetMesh;
	ElapsedTime = 0.0f;
	bTransitioning = true;

	World->GetTimerManager().ClearTimer(MeshHideTimerHandle);

	CreateDynamicMaterials();
	SetDissolveAmount(0.0f);

	if (DeathVFX)
	{
		SpawnedVFX = UNiagaraFunctionLibrary::SpawnSystemAttached(
			DeathVFX,
			CachedMesh,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		);
	}

	UE_LOG(LogTemp, Warning, TEXT("EnemyTransition Started / Materials: %d / VFXComponent: %s"),
		DynamicMaterials.Num(),
		*GetNameSafe(SpawnedVFX));

	SetComponentTickEnabled(true);
}

void UEnemyTransitionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bTransitioning)
	{
		return;
	}

	ElapsedTime += DeltaTime;

	const float DissolveAlpha = FMath::Clamp(ElapsedTime / DissolveDuration, 0.0f, 1.0f);
	SetDissolveAmount(DissolveAlpha);

	if (DissolveAlpha >= 1.0f)
	{
		FinishTransition();
	}
}

void UEnemyTransitionComponent::CreateDynamicMaterials()
{
	DynamicMaterials.Empty();

	if (!CachedMesh)
	{
		return;
	}

	const int32 MaterialCount = CachedMesh->GetNumMaterials();
	for (int32 MaterialIndex = 0; MaterialIndex < MaterialCount; ++MaterialIndex)
	{
		UMaterialInterface* SourceMaterial = CachedMesh->GetMaterial(MaterialIndex);
		UMaterialInterface* MaterialForDissolve = SupportsDissolveParameter(SourceMaterial)
			                                          ? SourceMaterial
			                                          : DissolveFallbackMaterial.Get();

		if (!MaterialForDissolve)
		{
			MaterialForDissolve = SourceMaterial;
		}

		UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(MaterialForDissolve, this);
		if (DynamicMaterial)
		{
			CachedMesh->SetMaterial(MaterialIndex, DynamicMaterial);
			DynamicMaterials.Add(DynamicMaterial);
		}
	}
}

bool UEnemyTransitionComponent::SupportsDissolveParameter(UMaterialInterface* Material) const
{
	if (!Material)
	{
		return false;
	}

	float DissolveAmount = 0.0f;
	return Material->GetScalarParameterValue(
		FHashedMaterialParameterInfo(DissolveParameterName),
		DissolveAmount
	);
}

void UEnemyTransitionComponent::SetDissolveAmount(float Amount)
{
	for (UMaterialInstanceDynamic* DynamicMaterial : DynamicMaterials)
	{
		if (DynamicMaterial)
		{
			DynamicMaterial->SetScalarParameterValue(DissolveParameterName, Amount);
		}
	}
}

void UEnemyTransitionComponent::FinishTransition()
{	
	OnTransitionFinishedDelegate.Broadcast();
	
	bTransitioning = false;
	SetComponentTickEnabled(false);

	if (MeshHideDelayAfterDissolve <= 0.0f)
	{
		HideMesh();
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		HideMesh();
		return;
	}

	World->GetTimerManager().SetTimer(
		MeshHideTimerHandle,
		this,
		&UEnemyTransitionComponent::HideMesh,
		MeshHideDelayAfterDissolve,
		false
	);
}

void UEnemyTransitionComponent::HideMesh()
{
	if (!CachedMesh)
	{
		return;
	}

	if (SpawnedVFX)
	{
		SpawnedVFX->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	}

	CachedMesh->SetVisibility(false, false);
	CachedMesh->SetHiddenInGame(true, false);
	Deactivate();
}
