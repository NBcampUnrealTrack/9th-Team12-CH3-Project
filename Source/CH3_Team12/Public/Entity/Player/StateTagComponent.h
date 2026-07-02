// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "StateTagComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnCharacterStateTagChanged,
	FGameplayTag,
	StateTag,
	bool,
	bAdded
);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH3_TEAM12_API UStateTagComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStateTagComponent();

public:
	UPROPERTY(BlueprintAssignable, Category="Character|State")
	FOnCharacterStateTagChanged OnStateTagChanged;

public:
	UFUNCTION(BlueprintPure, Category="Character|State")
	bool HasStateTag(FGameplayTag StateTag) const;

	UFUNCTION(BlueprintPure, Category="Character|State")
	bool HasStateTagExact(FGameplayTag StateTag) const;

	UFUNCTION(BlueprintPure, Category="Character|State")
	bool HasAnyStateTags(const FGameplayTagContainer& TagsToCheck) const;

	UFUNCTION(BlueprintPure, Category="Character|State")
	bool HasAllStateTags(const FGameplayTagContainer& TagsToCheck) const;

	UFUNCTION(BlueprintCallable, Category="Character|State")
	void AddStateTag(FGameplayTag StateTag);

	UFUNCTION(BlueprintCallable, Category="Character|State")
	void RemoveStateTag(FGameplayTag StateTag);

	UFUNCTION(BlueprintCallable, Category="Character|State")
	void SetStateTag(FGameplayTag StateTag, bool bShouldAdd);

	UFUNCTION(BlueprintCallable, Category="Character|State")
	void ClearStateTags();

	UFUNCTION(BlueprintPure, Category="Character|State")
	FGameplayTagContainer GetStateTagsCopy() const;

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Character|State", meta=(AllowPrivateAccess="true"))
	FGameplayTagContainer StateTags;
		
};
