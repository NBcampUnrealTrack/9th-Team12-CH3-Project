#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCameraComponent.generated.h"


UENUM(BlueprintType)
enum class EPlayerCameraMode : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	LockOn UMETA(DisplayName = "LockOn")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH3_TEAM12_API UPlayerCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerCameraComponent();
	
public:
	void InitializeCamera(class USpringArmComponent* InCameraBoom, class UCameraComponent* InFollowCamera);
	void SetupNormalCamera();
	
private:
	UPROPERTY()
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY()
	TObjectPtr<class UCameraComponent> FollowCamera;

	UPROPERTY(EditAnywhere, Category = "Camera|Normal")
	float NormalTargetArmLength = 350.0f;

	UPROPERTY(EditAnywhere, Category = "Camera|Normal")
	bool bNormalUsePawnControlRotation = true;
};
