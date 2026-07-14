#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "Framework/Subsystem/KatanaGraphicManagerSubsystem.h"
#include "GraphicSettingsPresenter.generated.h"

class UGraphicSettingsWidget;
class UKatanaGraphicManagerSubsystem;

/**
 *
 */
UCLASS()
class CH3_TEAM12_API UGraphicSettingsPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	virtual void Initialize(UGraphicSettingsWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UKatanaGraphicManagerSubsystem> GraphicManagerSubsystem;

	UPROPERTY()
	TWeakObjectPtr<UGraphicSettingsWidget> GraphicSettingsWidget;

	UFUNCTION()
	void HandleWindowModeChanged(EKatanaWindowMode NewWindowMode) const;

	UFUNCTION()
	void HandleResolutionChanged(FIntPoint NewResolution) const;

	UFUNCTION()
	void HandleQualityChanged(EKatanaGraphicQuality NewQuality) const;

	UFUNCTION()
	void HandleVSyncChanged(bool bIsVSync) const;

	UFUNCTION()
	void HandleBtnResetClicked() const;

	UFUNCTION()
	void HandleBtnDoneClicked() const;
};