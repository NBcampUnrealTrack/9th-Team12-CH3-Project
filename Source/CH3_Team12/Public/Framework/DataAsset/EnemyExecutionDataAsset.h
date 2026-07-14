#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyExecutionDataAsset.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct FEnemyExecutionData
{
	GENERATED_BODY()

	// 적이 재생할 몽타주
	UPROPERTY(EditAnywhere)
	UAnimMontage* EnemyMontage;

	UPROPERTY(EditAnywhere)
	FVector ExecutionOffset = FVector(-120.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere)
	float AcceptAngle = 60.f;
};

UCLASS()
class CH3_TEAM12_API UEnemyExecutionDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FEnemyExecutionData EnemyExecutionData;
};
