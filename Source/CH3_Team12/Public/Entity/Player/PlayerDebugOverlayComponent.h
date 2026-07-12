#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerDebugOverlayComponent.generated.h"

class APlayerCharacterBase;
class UStateTagComponent;
class UPlayerAttributeComponent;
class UCharacterMovementComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CH3_TEAM12_API UPlayerDebugOverlayComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerDebugOverlayComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

private:
	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UStateTagComponent> StateComponent;

	UPROPERTY()
	TObjectPtr<UPlayerAttributeComponent> AttributeComponent;

	UPROPERTY()
	TObjectPtr<UCharacterMovementComponent> MovementComponent;

	UPROPERTY(EditAnywhere, Category="Debug")
	bool bShowDebugOverlay = true;

	UPROPERTY(EditAnywhere, Category="Debug")
	int32 ScreenMessageKey = 10001;

	FString MakeDebugText() const;
};