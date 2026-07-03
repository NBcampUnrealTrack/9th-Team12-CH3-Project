#include "Framework/GameMode/KatanaMainMenuPlayerController.h"

#include "Engine/LocalPlayer.h"
#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"

void AKatanaMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	const FInputModeUIOnly InputMode;
	SetInputMode(InputMode);

	UKatanaUIManagerSubsystem* UIManager = GetLocalPlayer()->GetSubsystem<UKatanaUIManagerSubsystem>();
	if (!UIManager)
		return;

	UIManager->ShowMainMenuWidget();
}
