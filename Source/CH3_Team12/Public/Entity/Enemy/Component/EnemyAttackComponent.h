// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/CombatTypes.h"
#include "EnemyAttackComponent.generated.h"

struct FHitResult;
struct FHitBoxData;
struct FAttackAnimationData;
class UStateTagComponent;
class UEnemyAttackDataAsset;
class UAnimMontage;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackCanceled);

UENUM()
enum class EEnemyAttackPattern : uint8
{
	NormalAttack_1,
	NormalAttack_2,
	NormalAttack_3,
	FarStrongAttack,
	End,
};

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
	
	UFUNCTION(BlueprintCallable, Category= "Attack")
	void CancelAttack();
	
	void StartHitCheck();
	void AttackTrace();
	void EndHitCheck();
	
private:
	void ProcessHit(const FHitResult& InHitResult, const FAttackInfo& InAttackInfo);
	void ProcessDefenseResult(EDefenseResult InDefenseResult);
	
	void OnAttackParried();
	
	const FAttackAnimationData* GetCurrentPatternData();
	const FAttackAnimationData* GetSelectedPatternData(int32 InSelectedAction, int32 InSelectedPattern);
	
	USkeletalMeshComponent* GetOwnerSkeletalMeshComponent();
	
	void ResetComboCount();
	
protected:
	UFUNCTION()
	void StopAttackMontage();

private:
	FTimerHandle AttackCooldownTimerHandle;
	void ResetAttackCooldown();
	
	void OnAttackAnimationEnd();
	
public:
	UPROPERTY(BlueprintAssignable, Category="Attack")
	FOnAttackFinished OnAttackFinished;
	
	UPROPERTY(BlueprintAssignable, Category="Attack")
	FOnAttackCanceled OnAttackCanceled;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Animation|Data")
	TObjectPtr<UEnemyAttackDataAsset> AttackData;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Attack")
	bool bCanAttack = true;
	
	UPROPERTY()
	EEnemyAttackPattern CurrentPlayingPattern;
	
	UPROPERTY(EditAnywhere,	BlueprintReadOnly, Category = "Attack|Debug")
	bool bUseDebugColliderDraw = true;
private:
	UPROPERTY()
	TObjectPtr<UStateTagComponent> StateComponent;
	
	UPROPERTY(EditDefaultsOnly, Category="Combat|Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;
	
	UPROPERTY()
	TMap<FName, FVector> PreviousHitBoxCenters;
	
	TSet<TWeakObjectPtr<AActor>> HitActors;
	
	int8 ComboCount;
};
