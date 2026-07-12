#include "Framework/GameMode/KatanaPlayerController.h"

#include "EngineUtils.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Entity/Enemy/EnemyCharacterBase.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"

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

	const AEnemyCharacterBase* EnemyCharacterBase = FindEnemyCharacter();
	if (!EnemyCharacterBase)
		return;

	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(this);
	if (!UIManager)
		return;

	UIManager->ShowEnemyWidget(EnemyCharacterBase->GetEnemyAttributeComponent());
}

void AKatanaPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	PlayerCharacterBase = Cast<APlayerCharacterBase>(InPawn);
	if (!PlayerCharacterBase.IsValid())
		return;

	UPlayerAttributeComponent* AttributeComponent = PlayerCharacterBase->GetAttributeComponent();
	// AttributeComponentTest->ApplyHealthDamage(FMath::RandRange(0, 20));
	// AttributeComponentTest->ApplyPostureDamage(FMath::RandRange(20, 50));

	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(this);
	if (!UIManager)
		return;

	UIManager->ShowPlayerWidget(AttributeComponent);
}

void AKatanaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContextUI)
			{
				Subsystem->AddMappingContext(InputMappingContextUI, 2);
			}
		}
	}

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent)
		return;

	EnhancedInputComponent->BindAction(InGameMenuActon, ETriggerEvent::Started, this,
	                                   &AKatanaPlayerController::ToggleInGameMenu);

	UE_LOG(LogTemp, Warning, TEXT("UI 단축키 등록 완료!"));
}


AEnemyCharacterBase* AKatanaPlayerController::FindEnemyCharacter() const
{
	UE_LOG(LogTemp, Warning, TEXT("FindEnemyCharacter! 1"));

	const UWorld* World = GetWorld();
	if (!World) return nullptr;

	for (TActorIterator<AEnemyCharacterBase> It(World); It; ++It)
	{
		AEnemyCharacterBase* FoundActor = *It;
		if (!FoundActor)
			continue;
		UE_LOG(LogTemp, Warning, TEXT("FindEnemyCharacter! 2"));
		return  FoundActor;
	}

	UE_LOG(LogTemp, Warning, TEXT("FindEnemyCharacter! 3"));
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