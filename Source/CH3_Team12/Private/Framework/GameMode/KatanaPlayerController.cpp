#include "Framework/GameMode/KatanaPlayerController.h"

#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Entity/Enemy/EnemyCharacterBase.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"
#include "UserSettings/EnhancedInputUserSettings.h"

APlayerCharacterBase* AKatanaPlayerController::GetPlayerCharacter() const
{
	if (!PlayerCharacterBase.IsValid())
		return nullptr;

	return PlayerCharacterBase.Get();
}

void AKatanaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = false;
	const FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	AEnemyCharacterBase* EnemyCharacterBase = FindEnemyCharacter();
	if (!EnemyCharacterBase)
		return;

	// TODO 죽었을 경우 패배 처리

	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(this);
	if (!UIManager)
		return;

	UIManager->ShowEnemyWidget(EnemyCharacterBase);
}

void AKatanaPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PlayerCharacterBase = Cast<APlayerCharacterBase>(InPawn);
	if (!PlayerCharacterBase.IsValid())
		return;

	// TODO 죽었을 경우 패배 처리

	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(this);
	if (!UIManager)
		return;

	UIManager->ShowPlayerWidget(PlayerCharacterBase.Get());
}

void AKatanaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent)
		return;

	EnhancedInputComponent->BindAction(InGameMenuActon, ETriggerEvent::Started, this,
	                                   &AKatanaPlayerController::ToggleInGameMenu);

	UE_LOG(LogTemp, Warning, TEXT("UI 단축키 등록 완료!"));
}


AEnemyCharacterBase* AKatanaPlayerController::FindEnemyCharacter() const
{
	const UWorld* World = GetWorld();
	if (!World) return nullptr;

	for (TActorIterator<AEnemyCharacterBase> It(World); It; ++It)
	{
		AEnemyCharacterBase* FoundActor = *It;
		if (!FoundActor)
			continue;
		return FoundActor;
	}

	return nullptr;
}

// ReSharper disable once CppMemberFunctionMayBeConst
void AKatanaPlayerController::ToggleInGameMenu(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("메뉴 단축키 눌림!"));

	UKatanaUIManagerSubsystem* UIManager = GetLocalPlayer()->GetSubsystem<UKatanaUIManagerSubsystem>();
	if (!UIManager)
		return;

	if (!UIManager->HasInGameMenuWidget())
	{
		UIManager->ShowInGameMenuWidget();

		SetPause(true);

		bShowMouseCursor = true;
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);
	}
	else
	{
		UIManager->HideInGameMenuWidget();

		SetPause(false);

		bShowMouseCursor = false;
		const FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}
}
