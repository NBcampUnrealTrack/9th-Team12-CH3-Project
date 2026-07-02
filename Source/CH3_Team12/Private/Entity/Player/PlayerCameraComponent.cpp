#include "Entity/Player/PlayerCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

UPlayerCameraComponent::UPlayerCameraComponent()
{
}

void UPlayerCameraComponent::InitializeCamera(USpringArmComponent* InCameraBoom, UCameraComponent* InFollowCamera)
{
	CameraBoom = InCameraBoom;
	FollowCamera = InFollowCamera;
}

void UPlayerCameraComponent::SetupNormalCamera()
{
	if (CameraBoom == nullptr || FollowCamera == nullptr)
	{
		return;
	}
	
	CameraBoom->TargetArmLength = NormalTargetArmLength;
	CameraBoom->bUsePawnControlRotation = bNormalUsePawnControlRotation;
	FollowCamera->bUsePawnControlRotation = false;
}
