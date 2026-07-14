#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyExecutionDataAsset.generated.h"

class UCameraShakeBase;

UCLASS()
class CH3_TEAM12_API UEnemyExecutionDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
    
	// 플레이어가 재생할 몽타주
    UPROPERTY(EditAnywhere)
    UAnimMontage* PlayerExecutionMontage;
    
    // 적이 재생할 몽타주
    UPROPERTY(EditAnywhere)
    UAnimMontage* EnemyExecutionMontage;
    
	// 플레이어가 이동할 위치
	UPROPERTY(EditDefaultsOnly)
	FName PlayerExecutionSocket;
    
	// 처형 후 데미지
	UPROPERTY(EditDefaultsOnly)
	float Damage = 999999.f;
	
    // 카메라 연출
    UPROPERTY(EditAnywhere)
    UCameraShakeBase* CameraShake;
};
