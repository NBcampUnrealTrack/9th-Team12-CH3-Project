#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenuWidget.generated.h"

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

	//TODO 인벤토리 위젯 추가

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnChildSoundSettings;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnChildGraphicsSettings;

	//설정 탭 관련
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> SettingsWidgetSwitcher;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USoundSettingsWidget> SoundSettingsWidget;

	//TODO 그래픽 설정 추가;

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
