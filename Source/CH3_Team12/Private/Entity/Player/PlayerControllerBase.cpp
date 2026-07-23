#include "Entity/Player/PlayerControllerBase.h"
#include "Engine/LocalPlayer.h"

APlayerControllerBase::APlayerControllerBase() :
	InputMappingContext(nullptr),
	MoveAction(nullptr),
	JumpAction(nullptr),
	LookAction(nullptr),
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
}