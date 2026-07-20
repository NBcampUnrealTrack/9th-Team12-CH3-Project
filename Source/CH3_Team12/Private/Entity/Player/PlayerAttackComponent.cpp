#include "Entity/Player/PlayerAttackComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/StateTagComponent.h"
#include "Entity/Player/PlayerLocomotionComponent.h"
#include "Entity/Player/PlayerAttributeComponent.h"
#include "Entity/Player/PlayerEquipmentComponent.h"
#include "Entity/Player/PlayerWeaponComponent.h"
#include "Entity/Player/PlayerCameraComponent.h"
#include "Entity/Enemy/EnemyCharacterBase.h"
#include "Entity/Enemy/Component/EnemyAttributeComponent.h"
#include "Entity/Enemy/Component/EnemyDefenseComponent.h"
#include "Framework/DataAsset/EnemyExecutionDataAsset.h"
#include "GameplayTags/CombatGameplayTags.h"
#include "Framework/DataAsset/PlayerAttackDataAsset.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Entity/Weapon/WeaponBase.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"
#include "Entity/Item/ItemInstance.h"
#include "Framework/DataAsset/WeaponDataAsset.h"

UPlayerAttackComponent::UPlayerAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.SetTickFunctionEnable(false);
}

void UPlayerAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<APlayerCharacterBase>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCombatComponent : OwnerCharacter is nullptr"));
		return;
	}

	StateComponent = OwnerCharacter->GetStateTagComponent();
	if (!StateComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCombatComponent : StateComponent is nullptr"));
		return;
	}
	
	AttributeComponent = OwnerCharacter->GetAttributeComponent();
	if (!AttributeComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCombatComponent : AttributeComponent is nullptr"));
		return;
	}
	
	EquipmentComponent = OwnerCharacter->GetEquipmentComponent();
	if (!EquipmentComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCombatComponent : EquipmentComponent is nullptr"));
		return;
	}
	
	WeaponComponent = OwnerCharacter->GetWeaponComponent();
	if (!WeaponComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCombatComponent : WeaponComponent is nullptr"));
		return;
	}
	LocomotionComponent = OwnerCharacter->GetLocomotionComponent();
	if (!LocomotionComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCombatComponent : LocoMotionComponent is nullptr"));
		return;
	}
}

void UPlayerAttackComponent::OnAttackMontageEnded(
	UAnimMontage* Montage,
	bool bInterrupted)
{
	if (Montage != CurrentAttackMontage)
	{
		return;
	}

	EndAttack();
}

void UPlayerAttackComponent::OpenAttackRecovery()
{
	if (!OwnerCharacter || !StateComponent)
	{
		return;
	}

	if (!IsAttacking())
	{
		return;
	}

	bDodgeCancelWindowOpen = true;
	bMoveCancelWindowOpen = true;
	bGuardCancelWindowOpen = true;

	StateComponent->RemoveStateTag(
		CombatTags::State_Movement_Locked
	);

	if (LocomotionComponent)
	{
		LocomotionComponent->RefreshMovementSettings();
		
		if (LocomotionComponent->TryConsumeBufferedDodge())
		{
			return;
		}
	}
}

const UPlayerAttackDataAsset* UPlayerAttackComponent::GetAttackData() const
{
	if (!EquipmentComponent)
	{
		return nullptr;
	}

	const UWeaponDataAsset* WeaponData = 
		EquipmentComponent->GetEquippedWeaponData();

	if (!WeaponData)
	{
		return nullptr;
	}
	
	return WeaponData->AttackData;
}

const FAttackDefinition* UPlayerAttackComponent::GetAttackDataByType(
	EAttackType AttackType) const
{
	const UPlayerAttackDataAsset* Data = GetAttackData();

	if (!Data)
	{
		return nullptr;
	}

	switch (AttackType)
	{
	case EAttackType::Light:
		return &Data->LightAttack;

	case EAttackType::Heavy:
		return &Data->HeavyAttack;

	case EAttackType::Jump:
		return &Data->JumpAttack;

	case EAttackType::Dash:
		return &Data->DashAttack;

	default:
		return nullptr;
	}
}

void UPlayerAttackComponent::Attack(const FInputActionValue& Value)
{
	if (!OwnerCharacter || !StateComponent)
	{
		return;
	}

	if (IsAttacking())
	{
		if (!CanContinueCombo())
		{
			return;
		}

		if (bComboWindow)
		{
			ContinueCombo();
		}
		else
		{
			bComboBuffered = true;
		}

		return;
	}

	const bool bIsJumpAttack =
		OwnerCharacter->GetCharacterMovement() &&
		(
			OwnerCharacter->GetCharacterMovement()->IsFalling() ||
			StateComponent->HasStateTagExact(
				CombatTags::State_Movement_JumpStarting
			)
		);

	if (!bIsJumpAttack && CanStartDashAttack())
	{
		const FAttackDefinition* DashAttackData =
			GetAttackDataByType(EAttackType::Dash);

		if (DashAttackData)
		{
			if (LocomotionComponent)
			{
				LocomotionComponent->CancelDodgeForAttack();
				LocomotionComponent->DoStopSprint();
			}

			bDashAttackWindowOpen = false;

			StateComponent->RemoveStateTag(
				CombatTags::State_Combat_Dodging
			);

			StateComponent->RemoveStateTag(
				CombatTags::State_Combat_Invincible
			);

			StateComponent->RemoveStateTag(
				CombatTags::State_Movement_Sprinting
			);

			StateComponent->RemoveStateTag(
				CombatTags::State_Movement_Locked
			);

			StartAttack(DashAttackData, false);
		}

		return;
	}

	if (!bIsJumpAttack)
	{
		if (!StateComponent->HasStateTag(
			CombatTags::State_Action_Executing))
		{
			if (AEnemyCharacterBase* Enemy = FindExecutionTarget())
			{
				StartExecution(Enemy);
				return;
			}
		}
	}

	if (!CanStartAttack())
	{
		return;
	}

	const FAttackDefinition* AttackData = nullptr;

	if (bIsJumpAttack)
	{
		AttackData = GetAttackDataByType(EAttackType::Jump);
	}
	else
	{
		AttackData = GetAttackDataByType(EAttackType::Light);
	}

	if (AttackData)
	{
		StartAttack(AttackData, bIsJumpAttack);
	}
}

void UPlayerAttackComponent::HeavyAttack(const FInputActionValue& Value)
{
	if (!CanStartAttack())
	{
		return;
	}

	StartAttack(
		GetAttackDataByType(EAttackType::Heavy),
		false
	);
}


void UPlayerAttackComponent::CancelAttackInternal()
{
	StopAttackRotation();

	ClearAttackCancelWindows();

	bDashAttackWindowOpen = false;
	bComboWindow = false;
	bComboBuffered = false;
	bCurrentAttackIsJumpAttack = false;
	ComboIndex = 0;

	if (WeaponComponent)
	{
		WeaponComponent->EndWeaponHitCheck();
	}

	UAnimMontage* MontageToStop = CurrentAttackMontage;

	CurrentAttackMontage = nullptr;
	CurrentAttackData = nullptr;
	CurrentStep = nullptr;

	if (StateComponent)
	{
		StateComponent->RemoveStateTag(
			CombatTags::State_Combat_Attacking
		);

		StateComponent->RemoveStateTag(
			CombatTags::State_Movement_Locked
		);
	}

	if (MontageToStop && OwnerCharacter)
	{
		if (USkeletalMeshComponent* Mesh = OwnerCharacter->GetMesh())
		{
			if (UAnimInstance* AnimInstance = Mesh->GetAnimInstance())
			{
				AnimInstance->Montage_Stop(
					0.08f,
					MontageToStop
				);
			}
		}
	}

	if (LocomotionComponent)
	{
		LocomotionComponent->RefreshMovementSettings();
	}
}

void UPlayerAttackComponent::CancelAttackForDodge()
{
	CancelAttackInternal();
}

void UPlayerAttackComponent::CancelAttackForMovement()
{
	CancelAttackInternal();

	if (GetWorld())
	{
		NextAttackAllowedTime =
			GetWorld()->GetTimeSeconds() + MovementCancelAttackLockout;
	}
}

void UPlayerAttackComponent::CancelAttackForHit()
{
	CancelAttackInternal();
}

void UPlayerAttackComponent::CancelAttackForDeath()
{
	CancelAttackInternal();
}

void UPlayerAttackComponent::CancelAttackForPostureBreak()
{
	CancelAttackInternal();
}

void UPlayerAttackComponent::CancelAttackForGuard()
{
	CancelAttackInternal();
}

void UPlayerAttackComponent::TickComponent(
	float DeltaTime, 
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(
		DeltaTime, 
		TickType,
		ThisTickFunction);

	FRotator NewRot =
		FMath::RInterpConstantTo(
			OwnerCharacter->GetActorRotation(),
			TargetAttackRotation,
			DeltaTime,
			720.f);

	OwnerCharacter->SetActorRotation(NewRot);

	if (NewRot.Equals(TargetAttackRotation, 0.1f))
	{
		OwnerCharacter->SetActorRotation(TargetAttackRotation);
		StopAttackRotation();
	}
}

bool UPlayerAttackComponent::CanStartAttack() const
{
	if (!OwnerCharacter || !StateComponent)
	{
		return false;
	}

	if (GetWorld() && GetWorld()->GetTimeSeconds() < NextAttackAllowedTime)
	{
		return false;
	}
	
	if (!EquipmentComponent->GetEquippedWeapon())
	{
		return false;
	}

	if (!StateComponent->HasStateTagExact(CombatTags::State_Combat_Armed))
	{
		return false;
	}
	
	FGameplayTagContainer BlockTags;
	BlockTags.AddTag(CombatTags::State_Combat_Dodging);
	BlockTags.AddTag(CombatTags::State_Combat_Guarding);
	BlockTags.AddTag(CombatTags::State_Combat_Parry);
	BlockTags.AddTag(CombatTags::State_Movement_Locked);
	BlockTags.AddTag(CombatTags::State_Hit_PostureBroken);
	BlockTags.AddTag(CombatTags::State_Hit_Dead);
	BlockTags.AddTag(CombatTags::State_Hit_Reacting);
	BlockTags.AddTag(CombatTags::State_Action_UsingItem);
	BlockTags.AddTag(CombatTags::State_Action_Executing);

	return !StateComponent->HasAnyStateTags(BlockTags);
}

bool UPlayerAttackComponent::IsAttacking() const
{
	if (!StateComponent)
	{
		return false;
	}

	return StateComponent->HasStateTagExact(
		CombatTags::State_Combat_Attacking
	);
}

bool UPlayerAttackComponent::CanContinueCombo() const
{
	if (!CurrentAttackData)
	{
		return false;
	}
	
	return CurrentAttackData->Steps.IsValidIndex(ComboIndex + 1); 
}

void UPlayerAttackComponent::StartAttack(
	const FAttackDefinition* AttackInfo,
	bool bIsJumpAttack)
{
	if (!OwnerCharacter ||
		!StateComponent ||
		!AttackInfo ||
		!AttackInfo->Montage ||
		AttackInfo->Steps.IsEmpty())
	{
		return;
	}

	UAnimInstance* AnimInstance =
		OwnerCharacter->GetMesh()->GetAnimInstance();

	if (!AnimInstance)
	{
		return;
	}

	bCurrentAttackIsJumpAttack = bIsJumpAttack;

	CurrentAttackData = AttackInfo;
	CurrentAttackMontage = AttackInfo->Montage;
	ComboIndex = 0;
	CurrentStep = &CurrentAttackData->Steps[ComboIndex];

	if (CurrentStep && 
		CurrentStep->bRotateToInput)
	{
		StartAttackRotation();
	}
	else
	{
		StopAttackRotation();
	}

	bComboWindow = false;
	bComboBuffered = false;
	ClearAttackCancelWindows();
	
	StateComponent->AddStateTag(
		CombatTags::State_Combat_Attacking
	);

	StateComponent->AddStateTag(
		CombatTags::State_Movement_Locked
	);

	const float Duration =
		AnimInstance->Montage_Play(
			AttackInfo->Montage,
			CurrentStep->PlayRate
		);

	if (Duration <= 0.f)
	{
		EndAttack();
		return;
	}

	AnimInstance->Montage_JumpToSection(
		CurrentStep->SectionName,
		AttackInfo->Montage
	);

	FOnMontageEnded Delegate;

	Delegate.BindUObject(
		this,
		&UPlayerAttackComponent::OnAttackMontageEnded
	);

	AnimInstance->Montage_SetEndDelegate(
		Delegate,
		AttackInfo->Montage
	);
}

void UPlayerAttackComponent::EndAttack()
{
	StopAttackRotation();
	
	WeaponComponent->EndWeaponHitCheck();

	CurrentAttackMontage = nullptr;
	CurrentAttackData = nullptr;
	CurrentStep = nullptr;
	
	ComboIndex = 0;
	
	bComboWindow = false;
	bComboBuffered = false;
	ClearAttackCancelWindows();
	bDashAttackWindowOpen = false;
	bCurrentAttackIsJumpAttack = false;
	
	if (StateComponent)
	{
		StateComponent->RemoveStateTag(
			CombatTags::State_Combat_Attacking);
		StateComponent->RemoveStateTag(
			CombatTags::State_Movement_Locked);
	}

	if (OwnerCharacter)
	{
		if (LocomotionComponent)
		{
			LocomotionComponent->RefreshMovementSettings();
		}
	}
}

void UPlayerAttackComponent::OpenDashAttackWindow()
{
	bDashAttackWindowOpen = true;
}

void UPlayerAttackComponent::CloseDashAttackWindow()
{
	bDashAttackWindowOpen = false;
}

void UPlayerAttackComponent::ExecutionHitNotify()
{
	 if (!ExecutionTarget.IsValid())
        {
            return;
        }
    
        AEnemyCharacterBase* Enemy =
            ExecutionTarget.Get();
	
        if (!GetAttackData())
        {
            return;
        }
    
        Enemy->GetEnemyAttributeComponent()
            ->ApplyHealthDamage(GetAttackData()->ExecutionData.Damage);
}

void UPlayerAttackComponent::OpenComboWindow()
{
	if (!CanContinueCombo())
	{
		return;
	}

	bComboWindow = true;

	if (bComboBuffered)
	{
		bComboBuffered = false;
		ContinueCombo();
	}
}

void UPlayerAttackComponent::ContinueCombo()
{
	if (!OwnerCharacter ||
		!StateComponent ||
		!CurrentAttackData)
	{
		return;
	}

	if (!CanContinueCombo())
	{
		bComboBuffered = false;
		bComboWindow = false;
		return;
	}

	ClearAttackCancelWindows();

	++ComboIndex;

	CurrentStep = &CurrentAttackData->Steps[ComboIndex];
	
	bComboBuffered = false;
	bComboWindow = false;
	
	UAnimInstance* AnimInstance =
		OwnerCharacter->GetMesh()
			? OwnerCharacter->GetMesh()->GetAnimInstance()
			: nullptr;

	if (!AnimInstance)
	{
		return;
	}

	StateComponent->AddStateTag(
		CombatTags::State_Movement_Locked
	);
	
	if (CurrentStep && CurrentStep->bRotateToInput)
	{
		StartAttackRotation();
	}
	else
	{
		StopAttackRotation();
	}
	
	AnimInstance->Montage_SetPlayRate(
		CurrentAttackMontage,
		CurrentStep->PlayRate
	);

	AnimInstance->Montage_JumpToSection(
		CurrentStep->SectionName,
		CurrentAttackMontage
	);
}

void UPlayerAttackComponent::StartAttackRotation()
{
	if (!OwnerCharacter)
	{
		return;
	}

	// LockOn 상태에서는 이동 입력을 무시하고 락온 대상을 바라본다.
	if (UPlayerCameraComponent* CameraComponent =
		OwnerCharacter->GetPlayerCameraComponent())
	{
		AActor* LockOnTarget =
			CameraComponent->GetCurrentLockOnTarget();

		if (CameraComponent->IsLockOn() && IsValid(LockOnTarget))
		{
			FVector Direction =
				LockOnTarget->GetActorLocation()
				- OwnerCharacter->GetActorLocation();

			Direction.Z = 0.0f;

			if (!Direction.IsNearlyZero())
			{
				TargetAttackRotation =
					Direction.GetSafeNormal().Rotation();

				SetComponentTickEnabled(true);
				return;
			}
		}
	}

	// LockOn이 아닐 때만 기존처럼 이동 입력 방향으로 공격한다.
	if (!LocomotionComponent)
	{
		return;
	}

	FVector2D BufferedAttackInput =
		LocomotionComponent->GetLastMovementInput();

	if (BufferedAttackInput.IsNearlyZero())
	{
		return;
	}

	const FRotator ControlRot =
		OwnerCharacter->GetControlRotation();

	const FRotator YawRot(
		0.0f,
		ControlRot.Yaw,
		0.0f
	);

	FVector Direction =
		FRotationMatrix(YawRot).GetUnitAxis(EAxis::X)
		* BufferedAttackInput.X;

	Direction +=
		FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y)
		* BufferedAttackInput.Y;

	Direction.Z = 0.0f;

	if (!Direction.Normalize())
	{
		return;
	}

	TargetAttackRotation =
		Direction.Rotation();

	SetComponentTickEnabled(true);
}

void UPlayerAttackComponent::StartExecution(AEnemyCharacterBase* Enemy)
{
	if (!Enemy)
	{
		return;
	}
	
	const UPlayerAttackDataAsset* AttackData = 
		GetAttackData();
	
	const UEnemyExecutionDataAsset* EnemyData =  
		Enemy->GetExecutionData();
	
	ensure(AttackData);
	ensure(AttackData->ExecutionData.PlayerMontage);  
	
	if (!AttackData ||
		!EnemyData	||
		!AttackData->ExecutionData.PlayerMontage ||
		!Enemy->CanExecuted()
		)
	{
		return;
	}
	
	StateComponent->AddStateTag(CombatTags::State_Action_Executing);
	StateComponent->AddStateTag(CombatTags::State_Movement_Locked);
	
	StopAttackRotation();
	
	ExecutionTarget = Enemy;
	
	// 적 로컬 기준 오프셋 -> 월드 위치
	const FVector TargetLocation =
		Enemy->GetActorTransform().TransformPosition(
			EnemyData->EnemyExecutionData.ExecutionOffset);

	// 플레이어가 적을 바라보도록
	FRotator TargetRotation = Enemy->GetActorRotation();
	TargetRotation.Yaw += 180.f;

	OwnerCharacter->SetActorLocationAndRotation(
		TargetLocation,
		TargetRotation,
		false,
		nullptr,
		ETeleportType::TeleportPhysics);
	
	UAnimInstance* AnimInstance =
	OwnerCharacter->GetMesh()->GetAnimInstance();

	if (!AnimInstance)
	{
		return;
	}
	
	if (UPlayerCameraComponent* CameraComponent =
		OwnerCharacter->GetPlayerCameraComponent())
	{
		CameraComponent->StartExecutionCamera(ExecutionTarget.Get());
	}
	
	float Duration = AnimInstance->Montage_Play(
		AttackData->ExecutionData.PlayerMontage);
	
	if (Duration <= 0.0f)
	{
		if (UPlayerCameraComponent* CameraComponent =
			OwnerCharacter->GetPlayerCameraComponent())
		{
			CameraComponent->EndExecutionCamera();
		}

		StateComponent->RemoveStateTag(
			CombatTags::State_Action_Executing
		);

		StateComponent->RemoveStateTag(
			CombatTags::State_Movement_Locked
		);

		ExecutionTarget = nullptr;

		return;
	}
	
	FOnMontageEnded Delegate;
	Delegate.BindUObject(
		this,
		&UPlayerAttackComponent::OnExecutionMontageEnded);

	AnimInstance->Montage_SetEndDelegate(
		Delegate,
		AttackData->ExecutionData.PlayerMontage);
	
	Enemy->StartExecuted();
	
}

AEnemyCharacterBase* UPlayerAttackComponent::FindExecutionTarget() const
{
	if (!OwnerCharacter)
	{
		return nullptr;
	}

	const FVector Start =
		OwnerCharacter->GetActorLocation();

	const FVector End =
		Start +
		OwnerCharacter->GetActorForwardVector() *
		ExecutionTraceDistance;

	FHitResult Hit;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);

	bool bHit = GetWorld()->SweepSingleByChannel(
		Hit,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(ExecutionTraceRadius),
		Params);

	if (!bHit)
	{
		return nullptr;
	}

	AEnemyCharacterBase* Enemy =
		Cast<AEnemyCharacterBase>(Hit.GetActor());

	if (!Enemy)
	{
		return nullptr;
	}

	UEnemyAttributeComponent* Attribute =
		Enemy->GetEnemyAttributeComponent();

	if (!Attribute)
	{
		return nullptr;
	}

	if (!Attribute->IsPostureBroken())
	{
		return nullptr;
	}

	return Enemy;
}

void UPlayerAttackComponent::OnExecutionMontageEnded(
	UAnimMontage* Montage,
	bool bInterrupted)
{
	if (UPlayerCameraComponent* CameraComponent =
		OwnerCharacter->GetPlayerCameraComponent())
	{
		CameraComponent->EndExecutionCamera();
	}
	
	StateComponent->RemoveStateTag(
		CombatTags::State_Action_Executing);

	StateComponent->RemoveStateTag(
		CombatTags::State_Movement_Locked);
	
	ExecutionTarget = nullptr;

	StopAttackRotation();
	
	if (LocomotionComponent)
	{
		LocomotionComponent->RefreshMovementSettings();
	}
}

void UPlayerAttackComponent::StopAttackRotation()
{
	SetComponentTickEnabled(false);
	TargetAttackRotation = FRotator::ZeroRotator;
}

void UPlayerAttackComponent::HandleOwnerLanded(
	const FHitResult& Hit)
{
	if (!bCurrentAttackIsJumpAttack)
	{
		return;
	}

	if (!IsAttacking())
	{
		return;
	}

	CancelJumpAttackForLanding();
}

void UPlayerAttackComponent::CancelJumpAttackForLanding()
{
	CancelAttackInternal();
}

const FAttackHitData* UPlayerAttackComponent::GetCurrentHit(int32 HitIndex) const
{
	if (!CurrentStep)
		return nullptr;

	if (!CurrentStep->Hits.IsValidIndex(HitIndex))
		return nullptr;

	return &CurrentStep->Hits[HitIndex];
}

bool UPlayerAttackComponent::CanDodgeCancel() const
{
	return IsAttacking() && bDodgeCancelWindowOpen;
}

bool UPlayerAttackComponent::CanMoveCancel() const
{
	return IsAttacking() && bMoveCancelWindowOpen;
}

bool UPlayerAttackComponent::CanGuardCancel() const
{
	return IsAttacking() && bGuardCancelWindowOpen;
}

bool UPlayerAttackComponent::ShouldUseDashAttack() const
{
	if (!OwnerCharacter || !StateComponent)
	{
		return false;
	}

	const bool bIsSprinting =
		StateComponent->HasStateTagExact(
			CombatTags::State_Movement_Sprinting
		);
	
	return bDashAttackWindowOpen || bIsSprinting;
}

bool UPlayerAttackComponent::CanStartDashAttack() const
{
	if (!OwnerCharacter || !StateComponent || !EquipmentComponent)
	{
		return false;
	}

	if (!ShouldUseDashAttack())
	{
		return false;
	}

	if (!EquipmentComponent->GetEquippedWeapon())
	{
		return false;
	}

	if (!StateComponent->HasStateTagExact(
		CombatTags::State_Combat_Armed))
	{
		return false;
	}

	FGameplayTagContainer BlockTags;
	BlockTags.AddTag(CombatTags::State_Combat_Guarding);
	BlockTags.AddTag(CombatTags::State_Combat_Parry);
	BlockTags.AddTag(CombatTags::State_Hit_PostureBroken);
	BlockTags.AddTag(CombatTags::State_Hit_Dead);
	BlockTags.AddTag(CombatTags::State_Hit_Reacting);
	BlockTags.AddTag(CombatTags::State_Action_UsingItem);
	BlockTags.AddTag(CombatTags::State_Action_Executing);

	return !StateComponent->HasAnyStateTags(BlockTags);
}

void UPlayerAttackComponent::ClearAttackCancelWindows()
{
	bDodgeCancelWindowOpen = false;
	bMoveCancelWindowOpen = false;
	bGuardCancelWindowOpen = false;
}

