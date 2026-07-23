#include "UI/Settings/InputSettingsPresenter.h"

#include "Framework/InputActionNames.h"
#include "Framework/Subsystem/KatanaInputManagerSubsystem.h"
#include "UI/Settings/InputSettingsWidget.h"

void UInputSettingsPresenter::Initialize(UInputSettingsWidget* InWidget)
{
	InputSettingsWidget = InWidget;

	if (!InputSettingsWidget.IsValid())
		return;

	InputSettingsWidget->OnKeyBindingChanged.BindDynamic(this, &UInputSettingsPresenter::HandleKeyBindingChanged);
	InputSettingsWidget->OnBtnResetClicked.BindDynamic(this, &UInputSettingsPresenter::HandleBtnResetClicked);
	InputSettingsWidget->OnBtnDoneClicked.BindDynamic(this, &UInputSettingsPresenter::HandleBtnDoneClicked);
	InputSettingsWidget->OnBtnBackClicked.BindDynamic(this, &UInputSettingsPresenter::HandleBtnBackClicked);

	InputManagerSubsystem = UKatanaInputManagerSubsystem::Get(this);
	if (!InputManagerSubsystem.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("UInputSettingsPresenter: InputManagerSubsystem is null"));
		return;
	}

	PendingKeyBindings = InputManagerSubsystem->GetCurrentKeyBindings();
	InputSettingsWidget->UpdateWidget(PendingKeyBindings);
}

void UInputSettingsPresenter::Dispose()
{
	if (!InputSettingsWidget.IsValid())
		return;

	InputSettingsWidget->OnKeyBindingChanged.Unbind();
	InputSettingsWidget->OnBtnResetClicked.Unbind();
	InputSettingsWidget->OnBtnDoneClicked.Unbind();
	InputSettingsWidget->OnBtnBackClicked.Unbind();
}

void UInputSettingsPresenter::HandleKeyBindingChanged(FName ActionName, FKey NewKey)
{
	if (!InputManagerSubsystem.IsValid())
		return;

	PendingKeyBindings.Add(ActionName, NewKey);
}

void UInputSettingsPresenter::HandleBtnResetClicked()
{
	if (!InputSettingsWidget.IsValid() || !InputManagerSubsystem.IsValid())
		return;

	PendingKeyBindings = InputManagerSubsystem->GetDefaultKeyBindings();
	InputSettingsWidget->UpdateWidget(PendingKeyBindings);
}

void UInputSettingsPresenter::HandleBtnDoneClicked()
{
	if (!InputSettingsWidget.IsValid() || !InputManagerSubsystem.IsValid())
		return;

	for (const auto& Elem : PendingKeyBindings)
	{
		InputManagerSubsystem->SetKeyBinding(Elem.Key, Elem.Value);
	}

	// Attack 키랑 HeavyAttack 키를 똑같이 바꾸기
	if (PendingKeyBindings.Contains(InputActionNames::Attack))
	{
		const FKey& HeavyAttackKey = PendingKeyBindings[InputActionNames::Attack];
		InputManagerSubsystem->SetKeyBinding(InputActionNames::HeavyAttack, HeavyAttackKey);
	}

	InputManagerSubsystem->ApplyAndSaveInputSettings();

	if (InputSettingsWidget->bDoneAfterCollapsed)
		InputSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
}

void UInputSettingsPresenter::HandleBtnBackClicked()
{
	InputSettingsWidget->SetVisibility(ESlateVisibility::Collapsed);
}
