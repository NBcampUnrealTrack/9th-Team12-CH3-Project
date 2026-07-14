#pragma once

#include "NativeGameplayTags.h"

namespace CombatTags
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Combat_Attacking);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Combat_Dodging);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Combat_Armed);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Combat_Guarding);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Combat_Parry);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Combat_Invincible);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_LockOn);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_Sprinting);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_Locked);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Movement_JumpStarting);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Hit_PostureBroken);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Hit_Dead);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Hit_Reacting);
	
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_Equipping);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Action_UsingItem);
}