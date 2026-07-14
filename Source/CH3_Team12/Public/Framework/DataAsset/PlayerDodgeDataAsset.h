#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerDodgeDataAsset.generated.h"

class UAnimMontage;

UENUM(BlueprintType)
enum class EPlayerEvadeStyle : uint8
{
	DodgeRoll,
	StepEvade
};

UENUM(BlueprintType)
enum class EDodgeDirection : uint8
{
	Forward,
	ForwardRight,
	Right,
	BackwardRight,
	Backward,
	BackwardLeft,
	Left,
	ForwardLeft
};

USTRUCT(BlueprintType)
struct FEvadeMontageData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName SectionName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PlayRate = 1.0f;

	bool IsValid() const
	{
		return Montage != nullptr;
	}
};

USTRUCT(BlueprintType)
struct FEvadeDirectionSet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FEvadeMontageData Forward;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FEvadeMontageData ForwardRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FEvadeMontageData Right;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FEvadeMontageData BackwardRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FEvadeMontageData Backward;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FEvadeMontageData BackwardLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FEvadeMontageData Left;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FEvadeMontageData ForwardLeft;

	const FEvadeMontageData* FindData(
		EDodgeDirection Direction
	) const;
};

UCLASS(BlueprintType)
class CH3_TEAM12_API UPlayerDodgeDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Evade")
	EPlayerEvadeStyle EvadeStyle = EPlayerEvadeStyle::DodgeRoll;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Evade|DodgeRoll")
	FEvadeDirectionSet DodgeRollSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Evade|StepEvade")
	FEvadeDirectionSet StepEvadeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Evade|Input")
	float DirectionDeadZone = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Evade|Input")
	EDodgeDirection NoInputDirection = EDodgeDirection::Forward;

public:
	const FEvadeMontageData* FindEvadeData(
		EDodgeDirection Direction
	) const;
};