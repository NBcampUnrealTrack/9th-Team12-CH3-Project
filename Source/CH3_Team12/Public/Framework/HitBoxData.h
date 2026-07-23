#pragma once

#include "CoreMinimal.h"
#include "HitBoxData.generated.h"

USTRUCT(BlueprintType)
struct FHitBoxData
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HitBox")
	FName ActiveHitSocket;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HitBox", meta = (ClampMin = 0, UIMin = 0))
	float TraceRadius = 150.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="HitBox", meta = (ClampMax = 50, UIMax = 50))
	int32 TraceSampleCount = 5;
};
