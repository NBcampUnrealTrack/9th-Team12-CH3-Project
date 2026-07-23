#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/UICommonTypes.h"
#include "GraphicSettingsWidget.generated.h"

class UButton;
class UComboBoxString;
class UCheckBox;

/**
 *
 */
UCLASS()
class CH3_TEAM12_API UGraphicSettingsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnWindowModeChanged OnWindowModeChanged;
	FOnResolutionChanged OnResolutionChanged;
	FOnQualityChanged OnQualityChanged;
	FOnBoolChanged OnVSyncChanged;
	FOnRefreshRateChanged OnRefreshRateChanged;

	FOnButtonClicked OnBtnResetClicked;
	FOnButtonClicked OnBtnDoneClicked;
	FOnButtonClicked OnBtnBackClicked;

	void UpdateWidget(const EKatanaWindowMode WindowMode, const FIntPoint Resolution,
	                          const EKatanaGraphicQuality Quality, const bool bVSync, const int32 RefreshRate);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bDoneAfterCollapsed = false;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> ClickSound;

	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UComboBoxString> WindowModeComboBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UComboBoxString> ResolutionComboBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UComboBoxString> QualityComboBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCheckBox> VSyncCheckBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UComboBoxString> RefreshRateComboBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnReset;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> BtnDone;

	UPROPERTY(meta=(BindWidget, OptionalWidget=true))
	TObjectPtr<UButton> BtnBack;

	bool bIsUpdatingWidget = false;

	UFUNCTION()
	void InitializeComboBoxOptions() const;

	UFUNCTION()
	void HandleWindowModeSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType) const;

	UFUNCTION()
	void HandleResolutionSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType) const;

	UFUNCTION()
	void HandleQualitySelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType) const;

	UFUNCTION()
	void HandleVSyncCheckStateChanged(bool bIsChecked) const;

	UFUNCTION()
	void HandleRefreshRateSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType) const;

	UFUNCTION()
	void HandleBtnResetClicked() const;

	UFUNCTION()
	void HandleBtnDoneClicked() const;

	UFUNCTION()
	void HandleBtnBackClicked() const;

	void SetWindowModeWidget(const EKatanaWindowMode WindowMode) const;
	void SetResolutionWidget(const FIntPoint Resolution) const;
	void SetQualityWidget(const EKatanaGraphicQuality Quality) const;
	void SetVSyncWidget(const bool bIsVSync) const;
	void SetRefreshRateWidget(const int32 RefreshRate) const;
	void UpdateResolutionUIState(const EKatanaWindowMode WindowMode) const;
};
