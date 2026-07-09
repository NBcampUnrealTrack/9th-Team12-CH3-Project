#pragma once

#include "CoreMinimal.h"
#include "Framework/PresenterInterface.h"
#include "UObject/Object.h"
#include "MainMenuSettingsPresenter.generated.h"

class UUserWidget;
enum class ESlateVisibility : uint8;
class UMainMenuSettingsWidget;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UMainMenuSettingsPresenter : public UObject, public IPresenterInterface
{
	GENERATED_BODY()

public:
	void Initialize(UMainMenuSettingsWidget* InWidget);
	virtual void Dispose() override;

private:
	UPROPERTY()
	TWeakObjectPtr<UMainMenuSettingsWidget> MainMenuSettingsWidget;

	UFUNCTION()
	void HandleVisibilitySoundSettingsChanged(ESlateVisibility InVisibility, UUserWidget* InWidget);
};
