#include "Entity/Player/PlayerControllerBase.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"

APlayerControllerBase::APlayerControllerBase() :
	InputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
	SprintDodgeAction(nullptr),
	AttackAction(nullptr),
	HeavyAttackAction(nullptr),
	LockOnAction(nullptr),
	DodgeAction(nullptr),
	GuardAction(nullptr),
	EquipAction(nullptr),
	UseItemAction(nullptr)
{
}

void APlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (const ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = 
			LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}