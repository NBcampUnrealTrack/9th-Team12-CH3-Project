#include "GameplayTags/CombatGameplayTags.h"

namespace CombatTags
{
	UE_DEFINE_GAMEPLAY_TAG(
		State_Combat_Attacking,
		"State.Action.Attacking"
	);

	UE_DEFINE_GAMEPLAY_TAG(
		State_Combat_Dodging,
		"State.Action.Dodging"
	);

	UE_DEFINE_GAMEPLAY_TAG(
		State_Combat_Armed,
		"State.Combat.Armed"
	);

	UE_DEFINE_GAMEPLAY_TAG(
		State_Combat_Guarding,
		"State.Combat.Guarding"
	);

	UE_DEFINE_GAMEPLAY_TAG(
		State_Combat_Parry,
		"State.Combat.Parry"
	);

	UE_DEFINE_GAMEPLAY_TAG(
		State_Combat_Invincible,
		"State.Combat.Invincible"
	);

	UE_DEFINE_GAMEPLAY_TAG(
		State_Movement_LockOn,
		"State.Movement.LockOn"
	);

	UE_DEFINE_GAMEPLAY_TAG(
		State_Movement_Sprinting,
		"State.Movement.Sprinting"
	);

	UE_DEFINE_GAMEPLAY_TAG(
		State_Movement_Locked,
		"State.Movement.Locked"
	);
	
	UE_DEFINE_GAMEPLAY_TAG(
		State_Movement_JumpStarting,
		"State.Movement.JumpStarting"
	);

	UE_DEFINE_GAMEPLAY_TAG(
		CombatTags::State_Hit_PostureBroken,
		TEXT("State.Hit.PostureBroken")
	);

	UE_DEFINE_GAMEPLAY_TAG(
		CombatTags::State_Hit_Dead,
		TEXT("State.Hit.Dead")
	);

	UE_DEFINE_GAMEPLAY_TAG(
		State_Hit_Reacting,
		TEXT("State.Hit.Reacting")
	);
	
	UE_DEFINE_GAMEPLAY_TAG(
		State_Hit_Executed,
		TEXT("State.Hit.Executed")
	);
	
	UE_DEFINE_GAMEPLAY_TAG(
		State_Action_Equipping,
		TEXT("State.Action.Equipping")
	);
	
	UE_DEFINE_GAMEPLAY_TAG(
		State_Action_UsingItem,
		TEXT("State.Action.UsingItem")
	);
}
