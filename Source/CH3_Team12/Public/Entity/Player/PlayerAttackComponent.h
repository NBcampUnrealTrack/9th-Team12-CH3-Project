#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Combat/CombatTypes.h"
#include "Framework/DataAsset/PlayerAttackDataAsset.h"
#include "PlayerAttackComponent.generated.h"

class UPlayerLocomotionComponent;
class AEnemyCharacterBase;
class APlayerCharacterBase;
class UStateTagComponent;
class UPlayerAttributeComponent;
class UPlayerEquipmentComponent;
class UPlayerWeaponComponent;
class UPlayerCameraComponent;

struct FAttackDefinition;
struct FInputActionValue;
struct FHitResult;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CH3_TEAM12_API UPlayerAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerAttackComponent();

protected:
	virtual void BeginPlay() override;

public:
	// Input
	void Attack(const FInputActionValue& Value);
	void HeavyAttack(const FInputActionValue& Value);
	
	// Notify / NotifyState
	void OpenComboWindow();
	void OpenAttackRecovery();
	void EndAttack();
	void OpenDashAttackWindow();
	void CloseDashAttackWindow();
	void ExecutionHitNotify();
	
	const FAttackHitData* GetCurrentHit(int32 HitIndex) const;

	bool CanDodgeCancel() const { return bDodgeCancelWindowOpen; }
	bool CanMoveCancel() const { return bMoveCancelWindowOpen; }
	
	void CancelAttackInternal();

	void CancelAttackForDodge();
	void CancelAttackForMovement();
	void CancelAttackForHit();
	void CancelAttackForDeath();
	void CancelAttackForPostureBreak();

	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
	
	void HandleOwnerLanded(const FHitResult& Hit);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Execution")
	float ExecutionTraceDistance = 180.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Execution")
	float ExecutionTraceRadius = 60.f;
	
private:
	// Attack
	bool CanStartAttack() const;
	bool IsAttacking() const;
	bool CanContinueCombo() const;
	void StartAttack(
		const FAttackDefinition* AttackInfo,
		bool bIsJumpAttack
	);
	void ContinueCombo();
	void StartAttackRotation();
	/** 현재 장착한 무기의 AttackDataAsset */
	const UPlayerAttackDataAsset* GetAttackData() const;

	/** 공격 타입에 맞는 AttackData 반환 */
	const FAttackDefinition* GetAttackDataByType(
		EAttackType AttackType) const;
	
	void OnAttackMontageEnded(
		UAnimMontage* Montage,
		bool bInterrupted);
	
	// execution
	AEnemyCharacterBase* FindExecutionTarget() const;
	
	void StartExecution(AEnemyCharacterBase* Enemy);
	
	UPROPERTY()
	TWeakObjectPtr<AEnemyCharacterBase> ExecutionTarget;
	
	void OnExecutionMontageEnded(
		UAnimMontage* Montage,
		bool bInterrupted);

	// Components
	UPROPERTY()
	TObjectPtr<APlayerCharacterBase> OwnerCharacter;
	UPROPERTY()
	TObjectPtr<UStateTagComponent> StateComponent;
	UPROPERTY()
	TObjectPtr<UPlayerAttributeComponent> AttributeComponent;
	UPROPERTY()
	TObjectPtr<UPlayerEquipmentComponent> EquipmentComponent;
	UPROPERTY()
	TObjectPtr<UPlayerWeaponComponent> WeaponComponent;
	UPROPERTY()
	TObjectPtr<UPlayerLocomotionComponent> LocomotionComponent;
	// Runtime
	/** 현재 실행중인 공격 데이터 */
	const FAttackDefinition* CurrentAttackData = nullptr;
	
	const FAttackStepData* CurrentStep = nullptr;
	
	/** 현재 재생중인 몽타주 */
	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentAttackMontage;
	
	/** 현재 콤보 번호 */
	int32 ComboIndex = 0;

	/** 콤보 입력 가능 */
	bool bComboWindow = false;

	/** 다음 콤보 입력 버퍼 */
	bool bComboBuffered = false;
	
	bool bDodgeCancelWindowOpen = false;
	bool bDashAttackWindowOpen = false;
	bool bMoveCancelWindowOpen = false;

	FRotator TargetAttackRotation;
	
	void StopAttackRotation();

	bool bCurrentAttackIsJumpAttack = false;

	void CancelJumpAttackForLanding();

	bool ShouldUseDashAttack() const;
	bool CanStartDashAttack() const;
};
