// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyAttackComponent.generated.h"

class UEnemyAttackDataAsset;
class UAnimMontage;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackFinished);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CH3_TEAM12_API UEnemyAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEnemyAttackComponent();
	
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable, Category="Attack")
	bool ExecuteAttack(AActor* TargetActor, int32 SelectedAction, int32 SelectedPattern);

	UFUNCTION()
	void FinishAttack();

	UPROPERTY(BlueprintAssignable, Category="Attack")
	FOnAttackFinished OnAttackFinished;
	
	void StartHitCheck();
	void AttackTrace();
	void EndHitCheck();
	
protected:
	// UFUNCTION()
	// virtual void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	
	UFUNCTION()
	void StopAttackMontage();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|Data")
	TObjectPtr<UEnemyAttackDataAsset> AttackData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
	bool bCanAttack = true;

private:
	void ResetComboCount();
	int8 ComboCount;
	
private:
	FTimerHandle AttackCooldownTimerHandle;

	void ResetAttackCooldown();
};
