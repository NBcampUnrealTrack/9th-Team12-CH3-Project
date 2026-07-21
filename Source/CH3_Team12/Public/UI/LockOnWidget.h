#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LockOnWidget.generated.h"

class UCanvasPanelSlot;
class UImage;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API ULockOnWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateWidget(const FVector2D& ScreenPosition);

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidget> LockOnIcon;

	UPROPERTY()
	TObjectPtr<UCanvasPanelSlot> CanvasSlot;
};
