#include "CombatGameplayTags.h"

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
}
