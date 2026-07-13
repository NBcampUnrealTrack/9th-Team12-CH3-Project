#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "UIDataAsset.generated.h"

class UUserWidget;

USTRUCT(BlueprintType)
struct FUIWidgetInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName WidgetName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UUserWidget> WidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ZOrder = 0;
};

/**
 *
 */
UCLASS()
class CH3_TEAM12_API UUIDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<FName, FUIWidgetInfo> WidgetInfoMap;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FUIWidgetInfo> WidgetInfoList;
};
