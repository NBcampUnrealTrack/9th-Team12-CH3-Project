// ReSharper disable CppMemberFunctionMayBeConst
// ReSharper disable CppPassValueParameterByConstReference
#include "UI/Settings/InputSettingsWidget.h"

#include "Components/Button.h"
#include "Components/InputKeySelector.h"
#include "Framework/InputActionNames.h"

void UInputSettingsWidget::UpdateWidget(TMap<FName, FKey> KeyBindingMap)
{
	for (auto [ActionName, Key] : KeyBindingMap)
	{
		if (!KeySelectorMap.Contains(ActionName))
			continue;

		FInputChord Chord(Key);
		KeySelectorMap[ActionName]->SetSelectedKey(Chord);
	}
}

void UInputSettingsWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	const TPair<FName, TObjectPtr<UInputKeySelector>> KeySelectorPairs[] =
	{
		{ InputActionNames::MoveForward, MoveForwardKeySelector },
		{ InputActionNames::MoveBackward, MoveBackwardKeySelector },
		{ InputActionNames::MoveLeft, MoveLeftKeySelector },
		{ InputActionNames::MoveRight, MoveRightKeySelector },
		{ InputActionNames::Jump, JumpKeySelector },
		{ InputActionNames::Dodge, DodgeKeySelector },
		{ InputActionNames::Guard, GuardKeySelector },
		{ InputActionNames::Attack, AttackKeySelector },
		{ InputActionNames::LockOn, LockOnKeySelector },
		{ InputActionNames::UseItem, UseItemKeySelector },
		{ InputActionNames::Equip, EquipKeySelector },
		{ InputActionNames::Special, SpecialKeySelector },
		{ InputActionNames::InGameMenu, InGameMenuKeySelector },
	};

	KeySelectorMap.Reserve(UE_ARRAY_COUNT(KeySelectorPairs));

	for (const TPair<FName, TObjectPtr<UInputKeySelector>>& KeySelectorPair : KeySelectorPairs)
	{
		KeySelectorMap.Add(KeySelectorPair.Key, KeySelectorPair.Value);
	}

	MoveForwardKeySelector->OnKeySelected.AddDynamic(this, &UInputSettingsWidget::HandleMoveForwardKeySelected);
	MoveBackwardKeySelector->OnKeySelected.AddDynamic(this, &UInputSettingsWidget::HandleMoveBackwardKeySelected);
	MoveLeftKeySelector->OnKeySelected.AddDynamic(this, &UInputSettingsWidget::HandleMoveLeftKeySelected);
	MoveRightKeySelector->OnKeySelected.AddDynamic(this, &UInputSettingsWidget::HandleMoveRightKeySelected);
	JumpKeySelector->OnKeySelected.AddDynamic(this, &UInputSettingsWidget::HandleJumpKeySelected);
	DodgeKeySelector->OnKeySelected.AddDynamic(this, &UInputSettingsWidget::HandleDodgeKeySelected);
	GuardKeySelector->OnKeySelected.AddDynamic(this, &UInputSettingsWidget::HandleGuardKeySelected);
	AttackKeySelector->OnKeySelected.AddDynamic(this, &UInputSettingsWidget::HandleAttackKeySelected);
	LockOnKeySelector->OnKeySelected.AddDynamic(this, &UInputSettingsWidget::HandleLockOnKeySelected);
	UseItemKeySelector->OnKeySelected.AddDynamic(this, &UInputSettingsWidget::HandleUseItemKeySelected);
	EquipKeySelector->OnKeySelected.AddDynamic(this, &UInputSettingsWidget::HandleEquipKeySelected);
	SpecialKeySelector->OnKeySelected.AddDynamic(this, &UInputSettingsWidget::HandleSpecialKeySelected);
	InGameMenuKeySelector->OnKeySelected.AddDynamic(this, &UInputSettingsWidget::HandleInGameMenuKeySelected);

	BtnReset->OnClicked.AddDynamic(this, &UInputSettingsWidget::HandleBtnResetClicked);
	BtnDone->OnClicked.AddDynamic(this, &UInputSettingsWidget::HandleBtnDoneClicked);
}

void UInputSettingsWidget::HandleMoveForwardKeySelected(FInputChord SelectedKey)
{
	(void)OnKeyBindingChanged.ExecuteIfBound(InputActionNames::MoveForward, SelectedKey.Key);
}

void UInputSettingsWidget::HandleMoveBackwardKeySelected(FInputChord SelectedKey)
{
	(void)OnKeyBindingChanged.ExecuteIfBound(InputActionNames::MoveBackward, SelectedKey.Key);
}

void UInputSettingsWidget::HandleMoveLeftKeySelected(FInputChord SelectedKey)
{
	(void)OnKeyBindingChanged.ExecuteIfBound(InputActionNames::MoveLeft, SelectedKey.Key);
}

void UInputSettingsWidget::HandleMoveRightKeySelected(FInputChord SelectedKey)
{
	(void)OnKeyBindingChanged.ExecuteIfBound(InputActionNames::MoveRight, SelectedKey.Key);
}

void UInputSettingsWidget::HandleJumpKeySelected(FInputChord SelectedKey)
{
	(void)OnKeyBindingChanged.ExecuteIfBound(InputActionNames::Jump, SelectedKey.Key);
}

void UInputSettingsWidget::HandleDodgeKeySelected(FInputChord SelectedKey)
{
	(void)OnKeyBindingChanged.ExecuteIfBound(InputActionNames::Dodge, SelectedKey.Key);
}

void UInputSettingsWidget::HandleGuardKeySelected(FInputChord SelectedKey)
{
	(void)OnKeyBindingChanged.ExecuteIfBound(InputActionNames::Guard, SelectedKey.Key);
}

void UInputSettingsWidget::HandleAttackKeySelected(FInputChord SelectedKey)
{
	(void)OnKeyBindingChanged.ExecuteIfBound(InputActionNames::Attack, SelectedKey.Key);
}

void UInputSettingsWidget::HandleLockOnKeySelected(FInputChord SelectedKey)
{
	(void)OnKeyBindingChanged.ExecuteIfBound(InputActionNames::LockOn, SelectedKey.Key);
}

void UInputSettingsWidget::HandleUseItemKeySelected(FInputChord SelectedKey)
{
	(void)OnKeyBindingChanged.ExecuteIfBound(InputActionNames::UseItem, SelectedKey.Key);
}

void UInputSettingsWidget::HandleEquipKeySelected(FInputChord SelectedKey)
{
	(void)OnKeyBindingChanged.ExecuteIfBound(InputActionNames::Equip, SelectedKey.Key);
}

void UInputSettingsWidget::HandleSpecialKeySelected(FInputChord SelectedKey)
{
	(void)OnKeyBindingChanged.ExecuteIfBound(InputActionNames::Special, SelectedKey.Key);
}

void UInputSettingsWidget::HandleInGameMenuKeySelected(FInputChord SelectedKey)
{
	(void)OnKeyBindingChanged.ExecuteIfBound(InputActionNames::InGameMenu, SelectedKey.Key);
}

void UInputSettingsWidget::HandleBtnResetClicked() const
{
	(void)OnBtnResetClicked.ExecuteIfBound();
}

void UInputSettingsWidget::HandleBtnDoneClicked() const
{
	(void)OnBtnDoneClicked.ExecuteIfBound();
}
