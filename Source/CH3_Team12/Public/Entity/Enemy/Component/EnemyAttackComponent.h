// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/CombatTypes.h"
#include "GameplayTagContainer.h"
#include "EnemyAttackComponent.generated.h"

struct FHitResult;
struct FHitBoxData;
struct FAttackAnimationData;
class UEnemyAttributeComponent;
class UStateTagComponent;
class UEnemyAttackDataAsset;
class UAnimMontage;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackFinished);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackCanceled);

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
	
	void StartHitCheck(const TArray<FHitBoxData>& HitBoxes);
	void AttackTrace();
	void EndHitCheck();
	
	bool CanAttack();
	
private:
	void ProcessHit(const FHitResult& InHitResult, const FAttackInfo& InAttackInfo);
	void ProcessDefenseResult(EDefenseResult InDefenseResult);
	
	void OnAttackParried();
	
	// const FAttackAnimationData* GetCurrentPatternData();
	// const FAttackAnimationData* GetSelectedPatternData(int32 InSelectedAction, int32 InSelectedPattern);
	
	USkeletalMeshComponent* GetOwnerSkeletalMeshComponent() const;
	
protected:
	UFUNCTION()
	void StopAttackMontage();

	UFUNCTION(BlueprintCallable, Category = "Attack | Data")
	void SetCurrentAttackData(int32 InAttackDataIndex);
	
public:
	UPROPERTY(BlueprintAssignable, Category="Attack")
	FOnAttackFinished OnAttackFinished;
	
	UPROPERTY(BlueprintAssignable, Category="Attack")
	FOnAttackCanceled OnAttackCanceled;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data | Attack")
	TArray<TObjectPtr<UEnemyAttackDataAsset>> AttackDatas;
	
	UPROPERTY(BlueprintReadWrite, Category = "Data | Attack")
	TObjectPtr<UEnemyAttackDataAsset> CurrentAttackData;
	
	TArray<FHitBoxData> CurrentHitBoxDatas;
	
	UPROPERTY(EditAnywhere,	BlueprintReadOnly, Category = "Attack|Debug")
	bool bUseDebugColliderDraw = true;
private:
	UPROPERTY()
	TObjectPtr<UEnemyAttributeComponent> AttributeComponent;
	
	UPROPERTY()
	TObjectPtr<UStateTagComponent> StateComponent;
	
	UPROPERTY(EditDefaultsOnly, Category="Combat|Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Pawn;
	
	UPROPERTY()
	TMap<FName, FVector> PreviousHitBoxCenters;
	
	TSet<TWeakObjectPtr<AActor>> HitActors;
};
