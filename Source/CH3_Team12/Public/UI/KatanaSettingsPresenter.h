#pragma once

#include "CoreMinimal.h"
#include "KatanaSettingsWidget.h"
#include "Framework/PresenterInterface.h"
#include "Framework/Subsystem/KatanaSoundManagerSubsystem.h"
#include "UObject/Object.h"
#include "KatanaSettingsPresenter.generated.h"

/**
 *
 */
UCLASS()
class CH3_TEAM12_API UKatanaSettingsPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	virtual void Initialize(UKatanaSoundManagerSubsystem* InSubsystem, UKatanaSettingsWidget* ActiveView);
	virtual void Dispose() override;
};
