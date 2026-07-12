#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/UICommonTypes.h"
#include "MainMenuSettingsWidget.generated.h"

class UButton;
class USoundSettingsWidget;

/**
 *
 */
UCLASS()
class CH3_TEAM12_API UMainMenuSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//닫기 키 입력 추가
	//단축키로 닫기 키를 누르게 유도 내부 들어가서는 결정과 뒤로 를 표시

	FOnButtonClicked OnBtnBackClicked;

	USoundSettingsWidget* GetSoundSettings() const;

protected:
	UFUNCTION()
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnBack;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnSoundSettings;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USoundSettingsWidget> SoundSettingsWidget;

	//TODO GraphicSettingsWidget 추가

	UFUNCTION()
	void HandleBtnBackClicked();

	UFUNCTION()
	void HandleBtnSoundSettingsClicked();

	UFUNCTION()
	void HandleVisibilitySoundSettingsChanged(ESlateVisibility InVisibility);
};
