#include "Framework/KatanaInputSettingsSaveGame.h"

#include "InputCoreTypes.h"

namespace InputActionNames
{
	const FName MoveForward(TEXT("MoveForward"));
	const FName MoveBackward(TEXT("MoveBackward"));
	const FName MoveLeft(TEXT("MoveLeft"));
	const FName MoveRight(TEXT("MoveRight"));
	const FName Jump(TEXT("Jump"));
	const FName Dodge(TEXT("Dodge"));
	const FName Guard(TEXT("Guard"));
	const FName Attack(TEXT("Attack"));
	const FName LockOn(TEXT("LockOn"));
	const FName UseItem(TEXT("UseItem"));
	const FName Equip(TEXT("Equip"));
	const FName Special(TEXT("Special"));
	const FName InGameMenu(TEXT("InGameMenu"));
}

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
		{ InputActionNames::Equip, EKeys::E },
		{ InputActionNames::Special, EKeys::LeftControl },
		{ InputActionNames::InGameMenu, EKeys::Tab },
	};
}
