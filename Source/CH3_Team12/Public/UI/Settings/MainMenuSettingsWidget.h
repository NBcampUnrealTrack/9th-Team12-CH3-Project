#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenuSettingsWidget.generated.h"

class UButton;
class USoundSettingsWidget;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnVisibilityChanged, ESlateVisibility, InVisibility, UUserWidget*, InWidget);

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

	FOnVisibilityChanged OnVisibilitySoundSettingsChanged;

protected:
	UFUNCTION()
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnSoundSettings;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USoundSettingsWidget> SoundSettingsWidget;

	//TODO GraphicSettingsWidget 추가

	UFUNCTION()
	void HandleClickedBtnSoundSettings();

	UFUNCTION()
	void HandleVisibilitySoundSettingsChanged(ESlateVisibility InVisibility);
};
