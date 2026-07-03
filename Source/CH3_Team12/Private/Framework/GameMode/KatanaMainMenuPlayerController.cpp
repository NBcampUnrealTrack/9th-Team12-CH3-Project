#include "Framework/GameMode/KatanaMainMenuPlayerController.h"

#include "Engine/LocalPlayer.h"
#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"

void AKatanaMainMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UKatanaUIManagerSubsystem* UIManager = GetLocalPlayer()->GetSubsystem<UKatanaUIManagerSubsystem>();
	UIManager->ShowMainMenuWidget();
}
