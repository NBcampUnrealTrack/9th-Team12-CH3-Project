#include "Framework/DataAsset/PlayerDodgeDataAsset.h"

const FEvadeMontageData* FEvadeDirectionSet::FindData(
	EDodgeDirection Direction) const
{
	switch (Direction)
	{
	case EDodgeDirection::Forward:
		return &Forward;

	case EDodgeDirection::ForwardRight:
		return &ForwardRight;

	case EDodgeDirection::Right:
		return &Right;

	case EDodgeDirection::BackwardRight:
		return &BackwardRight;

	case EDodgeDirection::Backward:
		return &Backward;

	case EDodgeDirection::BackwardLeft:
		return &BackwardLeft;

	case EDodgeDirection::Left:
		return &Left;

	case EDodgeDirection::ForwardLeft:
		return &ForwardLeft;

	default:
		return &Forward;
	}
}

const FEvadeMontageData* UPlayerDodgeDataAsset::FindEvadeData(
	EDodgeDirection Direction) const
{
	const FEvadeDirectionSet& SelectedSet =
		EvadeStyle == EPlayerEvadeStyle::StepEvade
			? StepEvadeSet
			: DodgeRollSet;

	const FEvadeMontageData* Data =
		SelectedSet.FindData(Direction);

	if (Data && Data->IsValid())
	{
		return Data;
	}

	const FEvadeMontageData* FallbackData =
		SelectedSet.FindData(NoInputDirection);

	if (FallbackData && FallbackData->IsValid())
	{
		return FallbackData;
	}

	return SelectedSet.FindData(EDodgeDirection::Forward);
}