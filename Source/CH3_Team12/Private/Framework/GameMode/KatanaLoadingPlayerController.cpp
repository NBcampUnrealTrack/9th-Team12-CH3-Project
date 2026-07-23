#include "Framework/GameMode/KatanaLoadingPlayerController.h"

#include "Engine/LocalPlayer.h"
#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"

void AKatanaLoadingPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	const FInputModeUIOnly InputMode;
	SetInputMode(InputMode);

	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(this);
	if (!UIManager)
		return;

	UIManager->ShowLoadingWidget();
}
