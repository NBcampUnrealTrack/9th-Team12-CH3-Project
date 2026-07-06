// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/AnimationAttackInterface.h"
#include "EnemyCharacterBase.generated.h"

class UEnemyAttackComponent;
class UBehaviorTree;

UCLASS()
class CH3_TEAM12_API AEnemyCharacterBase : public ACharacter, public IAnimationAttackInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components")
	TObjectPtr<UEnemyAttackComponent> EnemyAttackComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UBehaviorTree* GetBehaviorTreeAsset() const { return BehaviorTreeAsset; }

	UFUNCTION(BlueprintCallable)
	UEnemyAttackComponent* GetEnemyAttackComponent() const { return EnemyAttackComponent; }
	
	// Attack Animation Interface's Section
public:
	virtual void AttackAnimationEnd() override;
	virtual void AttackHitCheckStart() override;
	virtual void AttackHitCheckTick() override;
	virtual void AttackHitCheckEnd() override;
};
