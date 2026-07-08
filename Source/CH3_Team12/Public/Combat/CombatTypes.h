#pragma once

#include "CoreMinimal.h"
#include "CombatTypes.generated.h"

UENUM(BlueprintType)
enum class EAttackDirection : uint8
{
	Front,
	Left,
	Right,
	Back,
	Thrust,
	Overhead
};

USTRUCT(BlueprintType)
struct FAttackInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PostureDamage = 10.0f;

	// 플레이어 기준으로 어디서 들어오는 공격인지
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAttackDirection AttackDirection = EAttackDirection::Front;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeParried = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeGuarded = true;
};