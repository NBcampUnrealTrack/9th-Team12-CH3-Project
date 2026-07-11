#pragma once

#include "CoreMinimal.h"
#include "CombatTypes.generated.h"

UENUM(BlueprintType)
enum class EAttackSwingDirection : uint8
{
	None,
	LeftToRight,
	RightToLeft,
	Thrust,
	Overhead
};

UENUM(BlueprintType)
enum class EHitReactionDirection : uint8
{
	Front,
	Back,
	Left,
	Right,
};

USTRUCT(BlueprintType)
struct FAttackInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PostureDamage = 10.0f;

	// Enemy 공격 애니메이션 기준 방향
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttackSwingDirection SwingDirection =
		EAttackSwingDirection::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeParried = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeGuarded = true;
};

USTRUCT(BlueprintType)
struct FIncomingAttackContext
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AActor> Attacker = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FAttackInfo AttackInfo;

	UPROPERTY()
	FHitResult Hit;

	// 선택값. Enemy 쪽에서 계산 가능하면 넘김.
	UPROPERTY()
	FVector AttackWorldDirection = FVector::ZeroVector;
};


UENUM(BlueprintType)
enum class EDefenseResult : uint8
{
	None,
	Parry,
	Guard,
	Hit,
	Invincible
};