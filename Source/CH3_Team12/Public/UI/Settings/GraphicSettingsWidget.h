#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/UICommonTypes.h"
#include "Framework/Subsystem/KatanaGraphicManagerSubsystem.h" // Enum 사용을 위해 추가
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
    FOnQualityChanged    OnQualityChanged;
    FOnBoolChanged       OnVSyncChanged;

    FOnButtonClicked OnBtnResetClicked;
    FOnButtonClicked OnBtnDoneClicked;

    void SetWindowModeWidget(const EKatanaWindowMode WindowMode) const;
    void SetResolutionWidget(const FIntPoint Resolution) const;
    void SetQualityWidget(const EKatanaGraphicQuality Quality) const;
    void SetVSyncWidget(const bool bIsVSync) const;

    void ResetGraphics() const;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool bDoneAfterCollapsed = false;

protected:
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
    TObjectPtr<UButton> BtnReset;

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UButton> BtnDone;

    UFUNCTION()
    void InitializeComboBoxOptions() const;

    // --- UMG 이벤트 핸들러 ---
    UFUNCTION()
    void HandleWindowModeSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType) const;

    UFUNCTION()
    void HandleResolutionSelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType) const;

    UFUNCTION()
    void HandleQualitySelectionChanged(FString SelectedItem, ESelectInfo::Type SelectionType) const;

    UFUNCTION()
    void HandleVSyncCheckStateChanged(bool bIsChecked) const;

    UFUNCTION()
    void HandleBtnResetClicked() const;

    UFUNCTION()
    void HandleBtnDoneClicked() const;
};