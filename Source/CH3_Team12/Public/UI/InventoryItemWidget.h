#pragma once

#include "CoreMinimal.h"
#include "UICommonTypes.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemWidget.generated.h"

class UTextBlock;
class UImage;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UInventoryItemWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnTextButtonClicked OnTextButtonClicked;

	UFUNCTION()
	void UpdateData(const FText& Name, UTexture2D* Icon, int32 Count);

protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ImgIcon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TxtCount;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TxtName;

	void HandleLeftClick();
	void HandleRightClick();
};
