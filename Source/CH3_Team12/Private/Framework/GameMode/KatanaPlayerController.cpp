// ReSharper disable CppMemberFunctionMayBeConst
#include "Framework/GameMode/KatanaPlayerController.h"

#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Entity/Enemy/EnemyCharacterBase.h"
#include "Entity/Enemy/Component/EnemyAttributeComponent.h"
#include "Entity/Enemy/Component/EnemyTransitionComponent.h"
#include "Entity/Player/PlayerAttributeComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Framework/Subsystem/KatanaLevelSubsystem.h"
#include "Framework/Subsystem/KatanaStageRecordManagerSubsystem.h"
#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"
#include "Kismet/GameplayStatics.h"
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

	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(this);
	if (!UIManager)
		return;

	UIManager->ShowEnemyWidget(EnemyCharacterBase);

	EnemyCharacterBase->GetEnemyAttributeComponent()->OnEnemyDeath.AddDynamic(
		this, &AKatanaPlayerController::OnEnemyDeath);
	EnemyCharacterBase->GetEnemyTransitionComponent()->OnTransitionFinishedDelegate.AddDynamic(
		this, &AKatanaPlayerController::OnEnemyTransitionFinished);
}

void AKatanaPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PlayerCharacterBase = Cast<APlayerCharacterBase>(InPawn);
	if (!PlayerCharacterBase.IsValid())
		return;

	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(this);
	if (!UIManager)
		return;

	UIManager->ShowPlayerWidget(PlayerCharacterBase.Get());

	UKatanaStageRecordManagerSubsystem* RecordManager = UKatanaStageRecordManagerSubsystem::Get(this);
	if (!RecordManager)
		return;

	RecordManager->StartStageRecord(UKatanaLevelSubsystem::GetTargetMapName(this));

	PlayerCharacterBase->GetAttributeComponent()->OnDead.AddDynamic(this, &AKatanaPlayerController::OnPlayerDead);
}

void AKatanaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent)
		return;

	EnhancedInputComponent->BindAction(InGameMenuActon, ETriggerEvent::Started, this,
	                                   &AKatanaPlayerController::ToggleInGameMenu);
}

void AKatanaPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if (UKatanaStageRecordManagerSubsystem* RecordManager = UKatanaStageRecordManagerSubsystem::Get(this))
	{
		RecordManager->CancelStageRecord();
	}

	if (UKatanaUIManagerSubsystem* UIManagerSubsystem = UKatanaUIManagerSubsystem::Get(this))
	{
		UIManagerSubsystem->HidePlayerWidget();
		UIManagerSubsystem->HideEnemyWidget();
	}
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

void AKatanaPlayerController::OnEnemyDeath()
{
	UKatanaStageRecordManagerSubsystem* RecordManager = UKatanaStageRecordManagerSubsystem::Get(this);
	if (!RecordManager)
		return;

	RecordManager->FinishStageRecord(UKatanaLevelSubsystem::GetTargetMapName(this));
}

void AKatanaPlayerController::OnEnemyTransitionFinished()
{
	bShowMouseCursor = true;
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(this);
	if (!UIManager)
		return;

	UIManager->ShowStageResultWidget();
}

void AKatanaPlayerController::OnPlayerDead()
{
	bShowMouseCursor = true;
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(this);
	if (!UIManager)
		return;

	UIManager->ShowPlayerDeathWidget();
}
