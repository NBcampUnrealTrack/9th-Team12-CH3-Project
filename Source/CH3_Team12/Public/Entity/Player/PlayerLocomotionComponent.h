#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerLocomotionComponent.generated.h"

class ACharacter;
class UAnimMontage;
class APlayerCharacterBase;
struct FInputActionValue;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH3_TEAM12_API UPlayerLocomotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerLocomotionComponent();
	
	virtual void BeginPlay() override;
	
public:
	void DoJump(bool bStartJump) const;
	void DoMove(const FInputActionValue& value);
	void DoSprint(bool bStartSprint);
	void Look(const FInputActionValue& value);
	
	// getter
	FORCEINLINE float GetNormalWalkSpeed() const { return NormalWalkSpeed; }
	FORCEINLINE float GetSprintSpeed() const { return SprintSpeed; }
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion|Speeds")
	float NormalWalkSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Locomotion|Speeds")
	float SprintSpeed;
	
private:
	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;
};
