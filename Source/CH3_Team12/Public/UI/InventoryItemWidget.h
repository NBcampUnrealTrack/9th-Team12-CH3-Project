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
	FOnItemIdClicked OnMouseLeftClicked;
	FOnItemIdClicked OnMouseRightClicked;

	UFUNCTION()
	void UpdateData(const FPrimaryAssetId& InItemId, const FText& Name, UTexture2D* Icon, int32 Count);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USoundBase> ClickSound;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ImgIcon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TxtCount;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TxtName;

	UPROPERTY()
	FPrimaryAssetId ItemId;

	void HandleLeftClick();
	void HandleRightClick();
};
