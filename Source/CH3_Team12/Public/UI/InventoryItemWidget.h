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
	void SetInfo(UTexture2D* Texture, int32 Count, const FString& Name);

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> ImgIcon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TxtCount;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TxtName;
};
