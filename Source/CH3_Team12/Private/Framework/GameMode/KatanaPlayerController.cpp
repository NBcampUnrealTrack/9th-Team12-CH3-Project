#include "Framework/GameMode/KatanaPlayerController.h"

#include "Entity/Player/PlayerAttributeComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"

void AKatanaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = false;
	const FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void AKatanaPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	const APlayerCharacterBase* PlayerCharacter = Cast<APlayerCharacterBase>(InPawn);
	if (!PlayerCharacter)
		return;

	UPlayerAttributeComponent* AttributeComponent = PlayerCharacter->GetAttributeComponent();
	// AttributeComponentTest = AttributeComponent;

	UKatanaUIManagerSubsystem* UIManager = GetLocalPlayer()->GetSubsystem<UKatanaUIManagerSubsystem>();
	if (!UIManager)
		return;

	UIManager->ShowPlayerWidget(AttributeComponent);

	//TODO 여기서 록온 이벤트를 받아오도록 설정
}

// void AKatanaPlayerController::Tick(float DeltaSeconds)
// {
// 	Super::Tick(DeltaSeconds);
//
// 	if (Time > SetTime)
// 	{
// 		AttributeComponentTest->ApplyHealthDamage(FMath::RandRange(0, 20));
// 		AttributeComponentTest->ApplyPostureDamage(FMath::RandRange(20, 50));
// 		Time = 0.0f;
// 	}
//
// 	Time += DeltaSeconds;
// }