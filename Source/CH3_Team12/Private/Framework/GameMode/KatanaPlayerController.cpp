#include "Framework/GameMode/KatanaPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
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
	// AttributeComponentTest->ApplyHealthDamage(FMath::RandRange(0, 20));
	// AttributeComponentTest->ApplyPostureDamage(FMath::RandRange(20, 50));

	UKatanaUIManagerSubsystem* UIManager = GetLocalPlayer()->GetSubsystem<UKatanaUIManagerSubsystem>();
	if (!UIManager)
		return;

	UIManager->ShowPlayerWidget(AttributeComponent);

	//TODO 여기서 록온 이벤트를 받아오도록 설정
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