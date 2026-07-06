// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyAttackComponent.generated.h"


class UAnimMontage;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackFinished);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CH3_TEAM12_API UEnemyAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEnemyAttackComponent();

	UFUNCTION(BlueprintCallable, Category="Attack")
	bool ExecuteAttack(AActor* TargetActor, int32 SelectedAction, int32 SelectedPattern);

	UFUNCTION()
	void FinishAttack();

	UPROPERTY(BlueprintAssignable, Category="Attack")
	FOnAttackFinished OnAttackFinished;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
	float AttackRange = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
	float AttackCooldown = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
	bool bCanAttack = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack | Montage")
	TObjectPtr<UAnimMontage> NormalAttack0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack | Montage")
	TObjectPtr<UAnimMontage> NormalAttack1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack | Montage")
	TObjectPtr<UAnimMontage> StrongAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack | Montage")
	TObjectPtr<UAnimMontage> FarStrongAttack;
	
	UFUNCTION()
	void StopAttackMontage();

private:
	FTimerHandle AttackCooldownTimerHandle;

	void ResetAttackCooldown();
};
