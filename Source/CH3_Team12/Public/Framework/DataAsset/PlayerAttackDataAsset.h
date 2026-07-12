#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerAttackDataAsset.generated.h"

class UAnimMontage;

USTRUCT(BlueprintType)
struct FAttackStepData
{
	GENERATED_BODY()

	// 몽타주 Section 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SectionName;

	// 공격 데미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Damage = 25.0f;

	// 자세 데미지
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PostureDamage = 20.0f;

	// 재생속도
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PlayRate = 1.0f;
};

USTRUCT(BlueprintType)
struct FAttackDefinition
{
	GENERATED_BODY()

	// 사용할 몽타주
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage;

	// 콤보 데이터
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FAttackStepData> Steps;
};

UENUM(BlueprintType)
enum class EAttackType : uint8
{
	Light,
	Heavy
};

UCLASS(BlueprintType)
class CH3_TEAM12_API UPlayerAttackDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// 약공격
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAttackDefinition LightAttack;

	// 강공격
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAttackDefinition HeavyAttack;
};
