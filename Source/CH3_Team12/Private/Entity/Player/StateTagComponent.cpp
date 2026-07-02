#include "Entity/Player/StateTagComponent.h"

UStateTagComponent::UStateTagComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UStateTagComponent::HasStateTag(FGameplayTag StateTag) const
{
	if (!StateTag.IsValid())
	{
		return false;
	}

	return StateTags.HasTag(StateTag);
}

bool UStateTagComponent::HasStateTagExact(FGameplayTag StateTag) const
{
	if (!StateTag.IsValid())
	{
		return false;
	}

	return StateTags.HasTagExact(StateTag);
}

bool UStateTagComponent::HasAnyStateTags(
	const FGameplayTagContainer& TagsToCheck
) const
{
	return StateTags.HasAny(TagsToCheck);
}

bool UStateTagComponent::HasAllStateTags(
	const FGameplayTagContainer& TagsToCheck
) const
{
	return StateTags.HasAll(TagsToCheck);
}

void UStateTagComponent::AddStateTag(FGameplayTag StateTag)
{
	if (!StateTag.IsValid())
	{
		return;
	}

	if (StateTags.HasTagExact(StateTag))
	{
		return;
	}

	StateTags.AddTag(StateTag);

	OnStateTagChanged.Broadcast(
		StateTag,
		true
	);
}

void UStateTagComponent::RemoveStateTag(FGameplayTag StateTag)
{
	if (!StateTag.IsValid())
	{
		return;
	}

	if (!StateTags.HasTagExact(StateTag))
	{
		return;
	}

	StateTags.RemoveTag(StateTag);

	OnStateTagChanged.Broadcast(
		StateTag,
		false
	);
}

void UStateTagComponent::SetStateTag(
	FGameplayTag StateTag,
	bool bShouldAdd
)
{
	if (bShouldAdd)
	{
		AddStateTag(StateTag);
	}
	else
	{
		RemoveStateTag(StateTag);
	}
}

void UStateTagComponent::ClearStateTags()
{
	TArray<FGameplayTag> TagsToRemove;
	StateTags.GetGameplayTagArray(TagsToRemove);

	for (const FGameplayTag& StateTag : TagsToRemove)
	{
		RemoveStateTag(StateTag);
	}
}

FGameplayTagContainer UStateTagComponent::GetStateTagsCopy() const
{
	return StateTags;
}