// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/DataAsset/PlayerDefenseDataAsset.h"

UAnimMontage* UPlayerDefenseDataAsset::GetParryReactionMontage(
	EHitReactionDirection ReactionDirection) const
{
	switch (ReactionDirection)
	{
	case EHitReactionDirection::Left:
		return ParryLeftMontage
			? ParryLeftMontage
			: ParryRightMontage;

	case EHitReactionDirection::Right:
		return ParryRightMontage
			? ParryRightMontage
			: ParryLeftMontage;

	default:
		return ParryRightMontage
			? ParryRightMontage
			: ParryLeftMontage;
	}
}

UAnimMontage* UPlayerDefenseDataAsset::GetGuardHitMontage(EHitReactionDirection ReactionDirection) const
{
	switch (ReactionDirection)
	{
	case EHitReactionDirection::Left:
		return GuardHitLeftMontage
			? GuardHitLeftMontage
			: GuardHitRightMontage;

	case EHitReactionDirection::Right:
		return GuardHitRightMontage
			? GuardHitRightMontage
			: GuardHitLeftMontage;

	default:
		return GuardHitRightMontage
			? GuardHitRightMontage
			: GuardHitLeftMontage;
	}
}

UAnimMontage* UPlayerDefenseDataAsset::GetHitReactionMontage(EHitReactionDirection ReactionDirection) const
{
	switch (ReactionDirection)
	{
	case EHitReactionDirection::Left:
		return HitLeftMontage
			? HitLeftMontage
			: HitFrontMontage;

	case EHitReactionDirection::Right:
		return HitRightMontage
			? HitRightMontage
			: HitFrontMontage;

	case EHitReactionDirection::Back:
		return HitBackMontage
			? HitBackMontage
			: HitFrontMontage;

	case EHitReactionDirection::Front:
	default:
		return HitFrontMontage;
	}
}
