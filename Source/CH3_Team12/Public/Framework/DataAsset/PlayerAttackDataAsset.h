#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerAttackDataAsset.generated.h"

class UAnimMontage;

UENUM(BlueprintType)
enum class EAttackTraceType : uint8
{
	Weapon     UMETA(DisplayName="Weapon"),
	Sphere     UMETA(DisplayName="Sphere"),
	Capsule    UMETA(DisplayName="Capsule"),
	Box        UMETA(DisplayName="Box")
};

USTRUCT(BlueprintType)
struct FAttackHitData
{
	GENERATED_BODY()

	/** HP 데미지 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Damage = 25.f;

	/** 자세 데미지 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PostureDamage = 20.f;

	/** 어떤 방식으로 판정할지 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAttackTraceType TraceType = EAttackTraceType::Weapon;

	/** Weapon / Sphere 공용 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float TraceRadius = 8.f;

	/** Weapon Trace 샘플 개수 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 TraceSampleCount = 5;

	/** Capsule 전용 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CapsuleHalfHeight = 50.f;

	/** Box 전용 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector BoxExtent = FVector(30.f);

	/** 충돌 채널 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;
};

USTRUCT(BlueprintType)
struct FAttackStepData
{
	GENERATED_BODY()

	// 몽타주 Section 이름
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SectionName;

	UPROPERTY(EditAnywhere)
	TArray<FAttackHitData> Hits;
	
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
	Heavy,
	Jump,
	Dodge
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
	
	// 점프공격
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAttackDefinition JumpAttack;
	
	// 회피공격
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FAttackDefinition DodgeAttack;
};
