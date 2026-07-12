#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UInventoryItemWidget;
class UImage;
class UTextBlock;
class UVerticalBox;
/**
 *
 */
UCLASS()
class CH3_TEAM12_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UInventoryItemWidget* AddAndItemWidget();
	void ClearItemWidgets();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UInventoryItemWidget> ItemWidget;

	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ImgDetailIcon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TxtDetailName;

	UPROPERTY(meta=(BindWidget))
	TSoftObjectPtr<UTextBlock> TxtDetailCapacity;

	UPROPERTY(meta=(BindWidget))
	TSoftObjectPtr<UTextBlock> TxtDetailDescription;
};
