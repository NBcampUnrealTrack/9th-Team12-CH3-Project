#include "Framework/KatanaInputSettingsSaveGame.h"

#include "Framework/InputActionNames.h"
#include "InputCoreTypes.h"

UKatanaInputSettingsSaveGame::UKatanaInputSettingsSaveGame()
{
	KeyBindings =
	{
		{ InputActionNames::MoveForward, EKeys::W },
		{ InputActionNames::MoveBackward, EKeys::S },
		{ InputActionNames::MoveLeft, EKeys::A },
		{ InputActionNames::MoveRight, EKeys::D },
		{ InputActionNames::Jump, EKeys::SpaceBar },
		{ InputActionNames::Dodge, EKeys::LeftShift },
		{ InputActionNames::Guard, EKeys::RightMouseButton },
		{ InputActionNames::Attack, EKeys::LeftMouseButton },
		{ InputActionNames::LockOn, EKeys::MiddleMouseButton },
		{ InputActionNames::UseItem, EKeys::R },
		{ InputActionNames::Special, EKeys::LeftControl },
		{ InputActionNames::InGameMenu, EKeys::Tab },
	};
}
