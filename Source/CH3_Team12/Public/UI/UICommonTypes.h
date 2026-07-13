#pragma once

#include "CoreMinimal.h"

DECLARE_DYNAMIC_DELEGATE(FOnButtonClicked);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTextButtonClicked, const FText&, Text);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnVolumeChanged, const float, Volume);
