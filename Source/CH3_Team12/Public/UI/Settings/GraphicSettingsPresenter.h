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

	EKatanaWindowMode PendingWindowMode;
	FIntPoint PendingResolution;
	EKatanaGraphicQuality PendingQuality;
	bool bPendingVSync;
	int32 PendingRefreshRate;

	UFUNCTION()
	void HandleWindowModeChanged(EKatanaWindowMode NewWindowMode);

	UFUNCTION()
	void HandleResolutionChanged(FIntPoint NewResolution);

	UFUNCTION()
	void HandleQualityChanged(EKatanaGraphicQuality NewQuality);

	UFUNCTION()
	void HandleVSyncChanged(bool bIsVSync);

	UFUNCTION()
	void HandleRefreshRateChanged(int32 NewRefreshRate);

	UFUNCTION()
	void HandleBtnResetClicked();

	UFUNCTION()
	void HandleBtnDoneClicked() const;
};