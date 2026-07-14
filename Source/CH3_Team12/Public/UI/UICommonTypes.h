#pragma once

#include "CoreMinimal.h"
#include "Framework/Subsystem/KatanaGraphicManagerSubsystem.h"

DECLARE_DYNAMIC_DELEGATE(FOnButtonClicked);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnItemIdClicked, const FPrimaryAssetId&, Id);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnVolumeChanged, const float, Volume);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnWindowModeChanged, EKatanaWindowMode, WindowMode);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnResolutionChanged, FIntPoint, NewResolution);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnQualityChanged, EKatanaGraphicQuality, SelectedQuality);

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnBoolChanged, bool, bIsChecked);