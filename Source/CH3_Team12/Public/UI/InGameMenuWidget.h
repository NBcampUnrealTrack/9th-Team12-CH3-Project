#pragma once

#include "CoreMinimal.h"
#include "UICommonTypes.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenuWidget.generated.h"

class UInputSettingsWidget;
class UGraphicSettingsWidget;
class UInventoryWidget;
class USoundSettingsWidget;
class UWidgetSwitcher;
class UButton;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UInGameMenuWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UInventoryWidget* GetInventoryWidget() const;
	USoundSettingsWidget* GetSoundSettingsWidget() const;
	UGraphicSettingsWidget* GetGraphicSettingsWidget() const;
	UInputSettingsWidget* GetInputSettingsWidget() const;

	FOnButtonClicked OnBtnMainMenuClicked;

protected:
	UFUNCTION()
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnInventory;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnSettings;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnMainMenu;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	//인벤토리
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInventoryWidget> InventoryWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnChildSoundSettings;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnChildGraphicsSettings;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnChildInputSettings;

	//설정 탭 관련
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> SettingsWidgetSwitcher;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USoundSettingsWidget> SoundSettingsWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UGraphicSettingsWidget> GraphicSettingsWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInputSettingsWidget> InputSettingsWidget;

	UFUNCTION()
	void HandleBtnInventoryClicked();

	UFUNCTION()
	void HandleBtnSettingsClicked();

	UFUNCTION()
	void HandleBtnMainMenuClicked();

	UFUNCTION()
	void HandleBtnChildSoundSettingsClicked();

	UFUNCTION()
	void HandleBtnChildGraphicsSettingsClicked();

	UFUNCTION()
	void HandleBtnChildInputSettingsClicked();

	void WidgetSwitcherChanged(int32 ActiveWidgetIndex);
	void SettingsWidgetSwitcherChanged(int32 ActiveWidgetIndex);
};
