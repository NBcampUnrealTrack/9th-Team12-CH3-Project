// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interface/AnimationAttackInterface.h"
#include "PlayerCharacterBase.generated.h"

class UNiagaraSystem;
class APlayerControllerBase;
class USpringArmComponent;
class UCameraComponent;
class UCharacterMovementComponent;

class UStateTagComponent;
class UPlayerLocomotionComponent;
class UPlayerAttributeComponent;
class UPlayerCameraComponent;
class UPlayerAttackComponent;
class UPlayerInventoryComponent;
class UPlayerEquipmentComponent;
class UPlayerWeaponComponent;
class UPlayerDefenseComponent;
class UPlayerDebugOverlayComponent;
class UPlayerItemUseComponent;
class UCombatFeedbackComponent;
class UFootstepComponent;

UCLASS()
class CH3_TEAM12_API APlayerCharacterBase : public ACharacter, public IAnimationAttackInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerCharacterBase();
	UStateTagComponent* GetStateTagComponent() const;
	UPlayerLocomotionComponent* GetLocomotionComponent() const;
	UPlayerAttributeComponent* GetAttributeComponent() const;
	UPlayerCameraComponent* GetPlayerCameraComponent() const;
	UPlayerInventoryComponent* GetInventoryComponent() const;
	UPlayerEquipmentComponent* GetEquipmentComponent() const;
	UPlayerWeaponComponent* GetWeaponComponent() const;
	UPlayerDefenseComponent* GetDefenseComponent() const;
	UPlayerAttackComponent* GetAttackComponent() const;
	UPlayerItemUseComponent* GetItemUseComponent() const;
	UPlayerDebugOverlayComponent* GetDebugOverlayComponent() const;
	UCombatFeedbackComponent* GetCombatFeedbackComponent() const;
	UFootstepComponent* GetFootstepComponent() const;

	USpringArmComponent* GetCameraBoom() const;
	UCameraComponent* GetFollowCamera() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCameraComponent> FollowCamera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USkeletalMeshComponent> HeadMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStateTagComponent> StateTagComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPlayerLocomotionComponent> LocomotionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPlayerAttributeComponent> AttributeComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPlayerCameraComponent> CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPlayerInventoryComponent> InventoryComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPlayerItemUseComponent> ItemUseComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPlayerEquipmentComponent> EquipmentComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPlayerWeaponComponent> WeaponComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPlayerDefenseComponent> DefenseComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPlayerAttackComponent> AttackComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UPlayerDebugOverlayComponent> DebugOverlayComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCombatFeedbackComponent> CombatFeedbackComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UFootstepComponent> FootstepComponent;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Attack Animation Interface's Section
	virtual void AttackAnimationEnd() override;
	virtual void AttackHitCheckStart(int32 HitIndex) override;
	virtual void AttackHitCheckTick() override;
	virtual void AttackHitCheckEnd() override;
};
