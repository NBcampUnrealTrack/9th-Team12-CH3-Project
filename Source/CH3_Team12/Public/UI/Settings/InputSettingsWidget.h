#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/UICommonTypes.h"
#include "InputSettingsWidget.generated.h"

class UButton;
class UInputKeySelector; // 언리얼 내장 키 입력 위젯

UCLASS()
class CH3_TEAM12_API UInputSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnKeyBindingChanged OnKeyBindingChanged;
	FOnButtonClicked OnBtnResetClicked;
	FOnButtonClicked OnBtnDoneClicked;

	void UpdateWidget(TMap<FName, FKey> KeyBindingMap);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDoneAfterCollapsed = false;

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInputKeySelector> MoveForwardKeySelector;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInputKeySelector> MoveBackwardKeySelector;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInputKeySelector> MoveLeftKeySelector;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInputKeySelector> MoveRightKeySelector;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInputKeySelector> JumpKeySelector;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInputKeySelector> DodgeKeySelector;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInputKeySelector> GuardKeySelector;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInputKeySelector> AttackKeySelector;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInputKeySelector> LockOnKeySelector;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInputKeySelector> UseItemKeySelector;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInputKeySelector> EquipKeySelector;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInputKeySelector> SpecialKeySelector;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInputKeySelector> InGameMenuKeySelector;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnReset;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnDone;

	UFUNCTION()
	void HandleMoveForwardKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void HandleMoveBackwardKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void HandleMoveLeftKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void HandleMoveRightKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void HandleJumpKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void HandleDodgeKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void HandleGuardKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void HandleAttackKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void HandleLockOnKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void HandleUseItemKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void HandleEquipKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void HandleSpecialKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void HandleInGameMenuKeySelected(FInputChord SelectedKey);

	UFUNCTION()
	void HandleBtnResetClicked() const;

	UFUNCTION()
	void HandleBtnDoneClicked() const;

	UPROPERTY(Transient)
	TMap<FName, TObjectPtr<UInputKeySelector>> KeySelectorMap;
};