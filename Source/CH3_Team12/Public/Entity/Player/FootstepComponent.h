#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "FootstepComponent.generated.h"

class APlayerCharacterBase;
class USoundBase;
class UNiagaraSystem;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CH3_TEAM12_API UFootstepComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFootstepComponent();

protected:
	virtual void BeginPlay() override;

public:
	void PlayFootstep(FName FootSocketName);

private:
	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;

	UPROPERTY(EditDefaultsOnly, Category="Footstep")
	float TraceDistance = 50.0f;

	UPROPERTY(EditDefaultsOnly, Category="Footstep")
	float MinSpeedToPlay = 10.0f;

	UPROPERTY(EditDefaultsOnly, Category="Footstep")
	TObjectPtr<USoundBase> DefaultFootstepSound;

	UPROPERTY(EditDefaultsOnly, Category="Footstep")
	TObjectPtr<UNiagaraSystem> DefaultFootstepEffect;

	UPROPERTY(EditDefaultsOnly, Category="Footstep")
	float VolumeMultiplier = 1.0f;
	
	float LastFootstepTime = -100.0f;

	UPROPERTY(EditDefaultsOnly, Category="Footstep")
	float MinFootstepInterval = 0.08f;
};