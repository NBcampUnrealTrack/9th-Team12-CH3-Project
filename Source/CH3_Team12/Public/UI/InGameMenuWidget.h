#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenuWidget.generated.h"

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

protected:
	UFUNCTION()
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnInventory;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnSettings;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	//인벤토리
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInventoryWidget> InventoryWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnChildSoundSettings;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnChildGraphicsSettings;

	//설정 탭 관련
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> SettingsWidgetSwitcher;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USoundSettingsWidget> SoundSettingsWidget;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UGraphicSettingsWidget> GraphicSettingsWidget;

	UFUNCTION()
	void HandleBtnInventoryClicked();

	UFUNCTION()
	void HandleBtnSettingsClicked();

	UFUNCTION()
	void HandleBtnChildSoundSettingsClicked();

	UFUNCTION()
	void HandleBtnChildGraphicsSettingsClicked();

	void WidgetSwitcherChanged(int32 ActiveWidgetIndex);
	void SettingsWidgetSwitcherChanged(int32 ActiveWidgetIndex);
};
