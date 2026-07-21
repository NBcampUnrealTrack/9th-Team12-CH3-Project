// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/AnimationAttackInterface.h"
#include "EnemyCharacterBase.generated.h"


class UNiagaraSystem;
class UEnemyExecutionDataAsset;
class UAnimMontage;
class UStateTagComponent;
class UEnemyAttackComponent;
class UBehaviorTree;
class UEnemyAttributeComponent;
class UEnemyDefenseComponent;
class UEnemyTransitionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(
	FOnDestroyed
);

UCLASS()
class CH3_TEAM12_API AEnemyCharacterBase : public ACharacter, public IAnimationAttackInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacterBase();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;


	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	const UEnemyExecutionDataAsset* GetExecutionData() const { return ExecutionData; }

	UFUNCTION(BlueprintCallable)
	FString GetEnemyName() const { return EnemyName; }
	bool CanExecuted();
	void StartExecuted();

public:
	FOnDestroyed OnDestroyedDelegate;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	TObjectPtr<UEnemyAttackComponent> EnemyAttackComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStateTagComponent> StateTagComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UEnemyAttributeComponent> AttributeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UEnemyDefenseComponent> EnemyDefenseComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UEnemyTransitionComponent> EnemyTransitionComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy | Animation")
	TObjectPtr<UAnimMontage> GroggyMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy | Animation")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	float DestroyTime = 30.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
    FString EnemyName = TEXT("None");

	UPROPERTY(EditDefaultsOnly, Category="Enemy | VFX")
	TObjectPtr<UNiagaraSystem> DeathDisintegrationVFX;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	TObjectPtr<UEnemyExecutionDataAsset> ExecutionData;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy | Animation | Dead")
	FName ExecutionMontageIsDeadNotifyKey = TEXT("IsDead");

	UPROPERTY(EditDefaultsOnly, Category = "Enemy | Animation | Dead")
	FName ExecutionMontageLoopEndSectionKey = TEXT("LoopEnd");

protected:
	UFUNCTION(BlueprintCallable, Category = "AI")
	void StopAILogic();
	UFUNCTION(BlueprintCallable, Category = "AI")
	void ResumeAILogic();

	UFUNCTION(BlueprintCallable, Category = "AI")
	virtual void DestroyEnemy();

private:
	UFUNCTION()
	void HandlePostureBroken();
	void PlayGroggyMontage();
	void OnGroggyMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnDeath();

	void ProcessDeath();
	bool PlayDeathMontage();
	void StartDeathTransition();
	void SetEnemyDestroyTimer();

	void StartExecuted_Implement();
	bool PlayExecutedMontage();
	void OnExecutedMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnExecutedMontageDeadCheck(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

public:
	UBehaviorTree* GetBehaviorTreeAsset() const { return BehaviorTreeAsset; }

	UFUNCTION(BlueprintCallable)
	UEnemyAttackComponent* GetEnemyAttackComponent() const { return EnemyAttackComponent; }

	UStateTagComponent* GetStateTagComponent() const { return StateTagComponent; }
	UEnemyAttributeComponent* GetEnemyAttributeComponent() const { return AttributeComponent; }
	UEnemyDefenseComponent* GetEnemyDefenseComponent() const { return EnemyDefenseComponent; }
	UEnemyTransitionComponent* GetEnemyTransitionComponent() const { return EnemyTransitionComponent; }

	// Attack Animation Interface's Section
	virtual void AttackAnimationEnd() override;
	virtual void AttackHitCheckStart(int32 HitIndex, const TArray<FHitBoxData>& HitBoxes) override;
	virtual void AttackHitCheckTick() override;
	virtual void AttackHitCheckEnd() override;
};
