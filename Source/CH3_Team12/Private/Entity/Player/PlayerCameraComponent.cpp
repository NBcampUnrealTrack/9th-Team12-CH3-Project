#include "Entity/Player/PlayerCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Entity/Player/StateTagComponent.h"
#include "GameplayTags/CombatGameplayTags.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"

UPlayerCameraComponent::UPlayerCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerCameraComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerActor = Cast<APlayerCharacterBase>(GetOwner());

	if (OwnerActor == nullptr)
	{
		return;
	}
	
	CameraBoom = OwnerActor->FindComponentByClass<USpringArmComponent>();
	FollowCamera = OwnerActor->FindComponentByClass<UCameraComponent>();
	
	StateTagComponent = OwnerActor->GetStateTagComponent();
	
	check(StateTagComponent);
	
	StateTagComponent->AddStateTag(CombatTags::State_Combat_Armed);
	
	SetupNormalCamera();
}

// 노말 카메라 상태로 전환하고 기본 카메라 설정 복구
void UPlayerCameraComponent::SetupNormalCamera()
{
	if (OwnerActor == nullptr || CameraBoom == nullptr || FollowCamera == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(OwnerActor->GetController()))
	{
		PlayerController->SetIgnoreLookInput(false);
	}
	
	if (StateTagComponent->HasStateTag(CombatTags::State_Movement_LockOn))
	{
		StateTagComponent->RemoveStateTag(CombatTags::State_Movement_LockOn);
	}
	
	CurrentLockOnTarget = nullptr;
	
	// Normal 카메라 기본 값 적용
	CameraBoom->TargetArmLength = NormalTargetArmLength;
	CameraBoom->SetRelativeLocation(NormalCameraBoomRelativeLocation);
	CameraBoom->SetRelativeRotation(NormalCameraBoomRelativeRotation);
	CameraBoom->SocketOffset = NormalSocketOffset;
	CameraBoom->TargetOffset = NormalTargetOffset;

	CameraBoom->bUsePawnControlRotation = true;

	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bDoCollisionTest = true;

	FollowCamera->bUsePawnControlRotation = false;

	if (UCharacterMovementComponent* MovementComponent =
		OwnerActor->FindComponentByClass<UCharacterMovementComponent>())
	{
		MovementComponent->bOrientRotationToMovement = true;
		MovementComponent->bUseControllerDesiredRotation = false;
	}

	OwnerActor->bUseControllerRotationYaw = false;
}

// 락온 카메라 상태로 전환하고 캐릭터 회전 방식을 락온용으로 변경
void UPlayerCameraComponent::SetupLockOnCamera()
{
	if (OwnerActor == nullptr || CameraBoom == nullptr || FollowCamera == nullptr)
	{
		return;
	}

	if (CurrentLockOnTarget == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(OwnerActor->GetController()))
	{
		PlayerController->SetIgnoreLookInput(true);
	}
	
	StateTagComponent->AddStateTag(CombatTags::State_Movement_LockOn);
	
	
	// =========================
	// Camera Setting
	// =========================
	// 락온에서도 카메라 위치 세팅은 노말과 동일하게 유지한다.
	// TargetArmLength / SocketOffset / TargetOffset 직접 변경 금지.

	CameraBoom->bUsePawnControlRotation = bNormalUsePawnControlRotation;

	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bDoCollisionTest = true;

	FollowCamera->bUsePawnControlRotation = false;

	// =========================
	// Character Rotation Setting
	// =========================

	if (UCharacterMovementComponent* MovementComponent =
		OwnerActor->FindComponentByClass<UCharacterMovementComponent>())
	{
		// 락온 중에는 이동 방향이 아니라 타겟 방향을 바라봐야 한다.
		MovementComponent->bOrientRotationToMovement = false;
		MovementComponent->bUseControllerDesiredRotation = false;
	}

	OwnerActor->bUseControllerRotationYaw = false;
}

// 현재 락온 모드인지 확인
bool UPlayerCameraComponent::IsLockOnMode() const
{
	return StateTagComponent->HasStateTag(CombatTags::State_Movement_LockOn);
}

// 락온 상태면 해제, 아니면 락온 시도
void UPlayerCameraComponent::LockOn()
{
	if (IsLockOnMode())
	{
		ClearLockOn();
		return;
	}

	TryLockOn();
}

// 락온 대상을 비우고 노말 카메라로 복귀
void UPlayerCameraComponent::ClearLockOn()
{
	CurrentLockOnTarget = nullptr;
	SetupNormalCamera();
}

// 카메라 방향 기준으로 적을 탐색하고 락온 대상으로 설정
void UPlayerCameraComponent::TryLockOn()
{
	if (OwnerActor == nullptr)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (World == nullptr)
	{
		return;
	}

	if (CameraBoom == nullptr || FollowCamera == nullptr)
	{
		return;
	}

	const FVector Start = OwnerActor->GetActorLocation();

	FVector TraceDirection = FollowCamera->GetForwardVector();
	TraceDirection.Z = 0.0f;

	if (TraceDirection.IsNearlyZero())
	{
		return;
	}

	TraceDirection.Normalize();

	const FVector End = Start + TraceDirection * LockOnTraceDistance;

	TArray<FHitResult> HitResults;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerActor);

	const FCollisionShape CapsuleShape =
		FCollisionShape::MakeCapsule(LockOnTraceRadius, LockOnTraceHalfHeight);

	const bool bHit = World->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		ECC_Pawn,
		CapsuleShape,
		Params
	);

	bool bValidEnemyHit = false;

	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();

			if (HitActor == nullptr)
			{
				continue;
			}

			if (!HitActor->ActorHasTag(EnemyTagName))
			{
				//UE_LOG(LogTemp, Warning, TEXT("Hit Actor but not Enemy: %s"), *HitActor->GetName());
				continue;
			}

			bValidEnemyHit = true;
			CurrentLockOnTarget = HitActor;
			SetupLockOnCamera();

			//UE_LOG(LogTemp, Warning, TEXT("LockOn Target: %s"), *HitActor->GetName());
			break;
		}
	}

	//UE_LOG(LogTemp, Warning, TEXT("Sweep Hit: %d / HitCount: %d"), bHit, HitResults.Num());
	
// Debug 도구 얼만큼의 크기인지
/*#if ENABLE_DRAW_DEBUG
	DrawDebugCapsule(
		World,
		Start,
		LockOnTraceHalfHeight,
		LockOnTraceRadius,
		FQuat::Identity,
		bValidEnemyHit ? FColor::Green : FColor::Red,
		false,
		1.0f,
		0,
		2.0f
	);
#endif*/
}

// 카메라 모드에 따라 노말/락온 카메라 갱신
void UPlayerCameraComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (IsLockOnMode())
	{
		UpdateLockOnCamera(DeltaTime);
	}
	else
	{
		UpdateNormalCamera(DeltaTime);
	}
}

// 락온 중 변경된 카메라 값을 노말 기본값으로 부드럽게 복구
void UPlayerCameraComponent::UpdateNormalCamera(float DeltaTime)
{
	if (CameraBoom == nullptr)
	{
		return;
	}

	// =========================
	// Normal Camera Restore
	// =========================
	// 락온 중에 보정되었던 카메라 값을
	// 노말 카메라 기본값으로 부드럽게 되돌린다.

	CameraBoom->SetRelativeLocation(
		FMath::VInterpTo(
			CameraBoom->GetRelativeLocation(),
			NormalCameraBoomRelativeLocation,
			DeltaTime,
			CameraInterpSpeed
		)
	);

	CameraBoom->TargetArmLength = FMath::FInterpTo(
		CameraBoom->TargetArmLength,
		NormalTargetArmLength,
		DeltaTime,
		CameraInterpSpeed
	);

	CameraBoom->SocketOffset = FMath::VInterpTo(
		CameraBoom->SocketOffset,
		NormalSocketOffset,
		DeltaTime,
		CameraInterpSpeed
	);

	CameraBoom->TargetOffset = FMath::VInterpTo(
		CameraBoom->TargetOffset,
		NormalTargetOffset,
		DeltaTime,
		CameraInterpSpeed
	);
}

void UPlayerCameraComponent::UpdateLockOnCamera(float DeltaTime)
{
	if (OwnerActor == nullptr || CurrentLockOnTarget == nullptr)
	{
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(OwnerActor);
	if (OwnerPawn == nullptr)
	{
		return;
	}

	AController* OwnerController = OwnerPawn->GetController();
	if (OwnerController == nullptr)
	{
		return;
	}

	if (CameraBoom == nullptr || FollowCamera == nullptr)
	{
		return;
	}

	// 거리 계산
	const FVector OwnerLocation = OwnerActor->GetActorLocation();
	const FVector TargetLocation = CurrentLockOnTarget->GetActorLocation();
	const float Distance = FVector::Dist2D(OwnerLocation, TargetLocation);

	// 크기 계산
	FVector OwnerOrigin;
	FVector OwnerExtent;
	OwnerActor->GetActorBounds(true, OwnerOrigin, OwnerExtent);

	FVector TargetOrigin;
	FVector TargetExtent;
	CurrentLockOnTarget->GetActorBounds(true, TargetOrigin, TargetExtent);

	const float OwnerHeight = OwnerExtent.Z * 2.0f;
	const float TargetHeight = TargetExtent.Z * 2.0f;
	const float HeightDifference = TargetHeight - OwnerHeight;

	// 기본값
	float TargetArmLength = CameraBoom->TargetArmLength;

	// CurveVector의 RotationY 값을 Controller Pitch에 적용하기 위한 변수
	float TargetPitch = OwnerController->GetControlRotation().Pitch;

	// 크기에 따른 스프링암 길이 / 카메라 보정 조절
	if (HeightDifference > LockOnTargetHeightDiff)
	{
		// 큰 몬스터용 LocationZ / RotationY / SocketZ Curve
		if (LockOnLargeMonsterCameraOffsetByDistanceCurve)
		{
			const FVector CameraOffsetValue =
				LockOnLargeMonsterCameraOffsetByDistanceCurve->GetVectorValue(Distance);

			const float TargetLocationZ = CameraOffsetValue.X;
			const float TargetRotationY = CameraOffsetValue.Y;
			const float TargetSocketZ = CameraOffsetValue.Z;

			// RotationY는 SpringArm에 직접 넣지 않고 Controller Pitch에 넣기 위해 저장
			TargetPitch = TargetRotationY;

			// SpringArm 위치 Z 적용
			FVector NewRelativeLocation = CameraBoom->GetRelativeLocation();
			NewRelativeLocation.Z = FMath::FInterpTo(
				NewRelativeLocation.Z,
				TargetLocationZ,
				DeltaTime,
				LockOnCameraInterpSpeed
			);
			CameraBoom->SetRelativeLocation(NewRelativeLocation);

			// SocketOffset Z 적용
			FVector NewSocketOffset = CameraBoom->SocketOffset;
			NewSocketOffset.Z = FMath::FInterpTo(
				NewSocketOffset.Z,
				TargetSocketZ,
				DeltaTime,
				LockOnCameraInterpSpeed
			);
			CameraBoom->SocketOffset = NewSocketOffset;

			UE_LOG(LogTemp, Error,
				TEXT("Distance : %f, TargetArmLength : %f, HeightDifference : %f, LocationZ : %f, RotationY : %f, SocketZ : %f"),
				Distance,
				TargetArmLength,
				HeightDifference,
				TargetLocationZ,
				TargetRotationY,
				TargetSocketZ
			);
		}

		// 큰 몬스터용 ArmLength Curve
		if (LockOnLargeMonsterArmLengthByDistanceCurve)
		{
			TargetArmLength = LockOnLargeMonsterArmLengthByDistanceCurve->GetFloatValue(Distance);
		}
	}
	else if (HeightDifference > -LockOnTargetHeightDiff)
	{
		// 중간 몬스터용 CameraOffset Curve
		if (LockOnMediumMonsterCameraOffsetByDistanceCurve)
		{
			const FVector CameraOffsetValue =
				LockOnMediumMonsterCameraOffsetByDistanceCurve->GetVectorValue(Distance);

			const float TargetLocationZ = CameraOffsetValue.X;
			const float TargetRotationY = CameraOffsetValue.Y;
			const float TargetSocketZ = CameraOffsetValue.Z;

			// RotationY는 SpringArm에 직접 넣지 않고 Controller Pitch에 넣기 위해 저장
			TargetPitch = TargetRotationY;

			// SpringArm 위치 Z 적용
			FVector NewRelativeLocation = CameraBoom->GetRelativeLocation();
			NewRelativeLocation.Z = FMath::FInterpTo(
				NewRelativeLocation.Z,
				TargetLocationZ,
				DeltaTime,
				LockOnCameraInterpSpeed
			);
			CameraBoom->SetRelativeLocation(NewRelativeLocation);

			// SocketOffset Z 적용
			FVector NewSocketOffset = CameraBoom->SocketOffset;
			NewSocketOffset.Z = FMath::FInterpTo(
				NewSocketOffset.Z,
				TargetSocketZ,
				DeltaTime,
				LockOnCameraInterpSpeed
			);
			CameraBoom->SocketOffset = NewSocketOffset;
		}

		// 중간 몬스터용 ArmLength Curve
		if (LockOnMediumMonsterArmLengthByDistanceCurve)
		{
			TargetArmLength = LockOnMediumMonsterArmLengthByDistanceCurve->GetFloatValue(Distance);
		}
	}
	else
	{
		// 작은 몬스터용 CameraOffset Curve
		if (LockOnSmallMonsterCameraOffsetByDistanceCurve)
		{
			const FVector CameraOffsetValue =
				LockOnSmallMonsterCameraOffsetByDistanceCurve->GetVectorValue(Distance);

			const float TargetLocationZ = CameraOffsetValue.X;
			const float TargetRotationY = CameraOffsetValue.Y;
			const float TargetSocketZ = CameraOffsetValue.Z;

			// RotationY는 SpringArm에 직접 넣지 않고 Controller Pitch에 넣기 위해 저장
			TargetPitch = TargetRotationY;

			// SpringArm 위치 Z 적용
			FVector NewRelativeLocation = CameraBoom->GetRelativeLocation();
			NewRelativeLocation.Z = FMath::FInterpTo(
				NewRelativeLocation.Z,
				TargetLocationZ,
				DeltaTime,
				LockOnCameraInterpSpeed
			);
			CameraBoom->SetRelativeLocation(NewRelativeLocation);

			// SocketOffset Z 적용
			FVector NewSocketOffset = CameraBoom->SocketOffset;
			NewSocketOffset.Z = FMath::FInterpTo(
				NewSocketOffset.Z,
				TargetSocketZ,
				DeltaTime,
				LockOnCameraInterpSpeed
			);
			CameraBoom->SocketOffset = NewSocketOffset;
		}

		// 작은 몬스터용 ArmLength Curve
		if (LockOnSmallMonsterArmLengthByDistanceCurve)
		{
			TargetArmLength = LockOnSmallMonsterArmLengthByDistanceCurve->GetFloatValue(Distance);
		}
	}

	// 스프링암 길이 적용
	CameraBoom->TargetArmLength = FMath::FInterpTo(
		CameraBoom->TargetArmLength,
		TargetArmLength,
		DeltaTime,
		LockOnCameraInterpSpeed
	);

	// 카메라가 항상 락온 대상을 바라보도록 회전
	FVector YawDirection = TargetLocation - OwnerLocation;
	YawDirection.Z = 0.0f;

	if (!YawDirection.IsNearlyZero())
	{
		const FRotator TargetYawRotation = YawDirection.Rotation();
		const FRotator CurrentCameraRotation = OwnerController->GetControlRotation();

		const FRotator DesiredCameraRotation(
			TargetPitch,
			TargetYawRotation.Yaw,
			0.0f
		);

		const FRotator NewCameraRotation = FMath::RInterpTo(
			CurrentCameraRotation,
			DesiredCameraRotation,
			DeltaTime,
			LockOnCameraRotationInterpSpeed
		);

		OwnerController->SetControlRotation(NewCameraRotation);
	}

	// 캐릭터 몸 방향도 락온 대상을 바라보도록 회전
	FVector BodyDirection = TargetLocation - OwnerLocation;
	BodyDirection.Z = 0.0f;

	if (!BodyDirection.IsNearlyZero())
	{
		const FRotator TargetBodyRotation = BodyDirection.Rotation();
		const FRotator CurrentBodyRotation = OwnerActor->GetActorRotation();

		const FRotator NewBodyRotation = FMath::RInterpTo(
			CurrentBodyRotation,
			TargetBodyRotation,
			DeltaTime,
			LockOnBodyRotationInterpSpeed
		);

		OwnerActor->SetActorRotation(NewBodyRotation);
	}
}