#include "Framework/GameMode/KatanaLoadingPlayerController.h"

#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"
#include "Framework/Subsystem/KatanaLevelSubsystem.h"

void AKatanaLoadingPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	const FInputModeUIOnly InputMode;
	SetInputMode(InputMode);

	UKatanaUIManagerSubsystem* UIManager = GetLocalPlayer()->GetSubsystem<UKatanaUIManagerSubsystem>();
	if (!UIManager)
		return;

	UKatanaLevelSubsystem* LevelSubsystem = GetGameInstance()->GetSubsystem<UKatanaLevelSubsystem>();
	if (!LevelSubsystem)
		return;

	UIManager->ShowLoadingWidget(LevelSubsystem);
}
