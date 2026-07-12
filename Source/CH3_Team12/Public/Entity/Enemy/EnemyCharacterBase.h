// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/AnimationAttackInterface.h"
#include "EnemyCharacterBase.generated.h"

class UAnimMontage;
class UStateTagComponent;
class UEnemyAttackComponent;
class UBehaviorTree;
class UEnemyAttributeComponent;

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
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	TObjectPtr<UEnemyAttackComponent> EnemyAttackComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStateTagComponent> StateTagComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UEnemyAttributeComponent> AttributeComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Enemy | Animation")
	TObjectPtr<UAnimMontage> HitMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = "Enemy | Animation")
	TObjectPtr<UAnimMontage> DeadMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	float DestroyTime = 10.0f;
	
protected:
	UFUNCTION()
	void OnDeath();
	
	void PlayDeathMontage();
	void SetEnemyDestroyTimer();
	
	virtual void DestroyEnemy();
	
public:
	UBehaviorTree* GetBehaviorTreeAsset() const { return BehaviorTreeAsset; }

	UFUNCTION(BlueprintCallable)
	UEnemyAttackComponent* GetEnemyAttackComponent() const { return EnemyAttackComponent; }
	UStateTagComponent* GetStateTagComponent() const { return StateTagComponent;}
	UEnemyAttributeComponent* GetEnemyAttributeComponent() const { return AttributeComponent;}
	
	// Attack Animation Interface's Section
public:
	virtual void AttackAnimationEnd() override;
	virtual void AttackHitCheckStart() override;
	virtual void AttackHitCheckTick() override;
	virtual void AttackHitCheckEnd() override;
};
