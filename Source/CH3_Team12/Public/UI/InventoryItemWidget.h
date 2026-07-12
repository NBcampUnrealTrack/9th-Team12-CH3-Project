#pragma once

#include "CoreMinimal.h"
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
	UFUNCTION()
	void UpdateData(const FText& Name, UTexture2D* Icon, int32 Count);

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ImgIcon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TxtCount;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TxtName;
};
