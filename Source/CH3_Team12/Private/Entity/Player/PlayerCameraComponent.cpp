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
	
	CameraBoom->bUsePawnControlRotation = bNormalUsePawnControlRotation;

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



// 락온 대상의 거리와 크기에 따라 카메라 위치, 회전, 캐릭터 방향 갱신
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

	// =========================
	// 1. 위치 정보
	// =========================
	const FVector OwnerLocation = OwnerActor->GetActorLocation();
	const FVector TargetLocation = CurrentLockOnTarget->GetActorLocation();

	const float Distance = FVector::Dist2D(OwnerLocation, TargetLocation);

	// =========================
	// 2. 캐릭터 / 타겟 Bounds 정보
	// =========================
	FVector OwnerOrigin;
	FVector OwnerExtent;
	OwnerActor->GetActorBounds(true, OwnerOrigin, OwnerExtent);

	FVector TargetOrigin;
	FVector TargetExtent;
	CurrentLockOnTarget->GetActorBounds(true, TargetOrigin, TargetExtent);

	const float OwnerHeight = OwnerExtent.Z * 2.0f;
	const float TargetHeight = TargetExtent.Z * 2.0f;

	const float HeightDifference = TargetHeight - OwnerHeight;

	// 큰 보스인지 판단
	// 큰 보스가 아니면 전부 SmallTarget 세팅을 사용한다.
	const bool bIsLargeTarget = HeightDifference > LockOnTargetHeightDeadZone;

	// =========================
	// 3. 거리 기반 보정 비율 계산
	// =========================
	// 가까우면 1.0
	// 멀면 0.0
	const float DistanceAlpha = FMath::GetMappedRangeValueClamped(
		FVector2D(LockOnHeightCorrectionFarDistance, LockOnHeightCorrectionNearDistance),
		FVector2D(0.0f, 1.0f),
		Distance
	);

	// =========================
	// 4. SpringArm Length 보정
	// =========================
	float TargetArmLength = NormalTargetArmLength;

	if (bIsLargeTarget)
	{
		TargetArmLength = FMath::Lerp(
			LockOnLargeTargetArmLengthFar,
			LockOnLargeTargetArmLengthClose,
			DistanceAlpha
		);
	}
	else
	{
		TargetArmLength = FMath::Lerp(
			LockOnSmallTargetArmLengthFar,
			LockOnSmallTargetArmLengthClose,
			DistanceAlpha
		);
	}

	CameraBoom->TargetArmLength = FMath::FInterpTo(
		CameraBoom->TargetArmLength,
		TargetArmLength,
		DeltaTime,
		CameraInterpSpeed
	);

	// =========================
	// 5. RelativeLocation 보정
	// =========================
	// 멀 때는 NormalCameraBoomRelativeLocation.
	// 가까울 때만 LockOn 전용 RelativeLocation을 사용한다.
	FVector TargetRelativeLocation = NormalCameraBoomRelativeLocation;

	if (bIsLargeTarget)
	{
		TargetRelativeLocation = FMath::Lerp(
			NormalCameraBoomRelativeLocation,
			LockOnLargeTargetCloseRelativeLocation,
			DistanceAlpha
		);
	}
	else
	{
		TargetRelativeLocation = FMath::Lerp(
			NormalCameraBoomRelativeLocation,
			LockOnSmallTargetCloseRelativeLocation,
			DistanceAlpha
		);
	}

	FVector NewRelativeLocation = FMath::VInterpTo(
		CameraBoom->GetRelativeLocation(),
		TargetRelativeLocation,
		DeltaTime,
		CameraInterpSpeed
	);

	if (FVector::DistSquared(NewRelativeLocation, TargetRelativeLocation) < 1.0f)
	{
		NewRelativeLocation = TargetRelativeLocation;
	}

	CameraBoom->SetRelativeLocation(NewRelativeLocation);

	// =========================
	// 6. SocketOffset.Z 보정
	// =========================
	float HeightCorrection = 0.0f;

	if (bIsLargeTarget)
	{
		HeightCorrection = FMath::Lerp(
			LockOnLargeTargetFarSocketZOffset,
			LockOnLargeTargetCloseSocketZOffset,
			DistanceAlpha
		);
	}
	else
	{
		HeightCorrection = FMath::Lerp(
			LockOnSmallTargetFarSocketZOffset,
			LockOnSmallTargetCloseSocketZOffset,
			DistanceAlpha
		);
	}

	FVector TargetSocketOffset = NormalSocketOffset;
	TargetSocketOffset.Z += HeightCorrection;

	CameraBoom->SocketOffset = FMath::VInterpTo(
		CameraBoom->SocketOffset,
		TargetSocketOffset,
		DeltaTime,
		CameraInterpSpeed
	);
	
// =========================
// 7. 카메라 회전 보정
// =========================
// Yaw는 기본적으로 타겟을 바라본다.
// 단, 작은 타겟이 가까울 때는 타겟 위치를 캐릭터 쪽으로 더 당겨서
// 캐릭터와 타겟이 화면 중앙에 더 잘 들어오도록 보정한다.

FVector CameraLookTargetLocation = TargetLocation;

if (!bIsLargeTarget)
{
	// 작은 몬스터일 때만 가까운 거리 보정 적용
	// DistanceAlpha는 가까우면 1.0, 멀면 0.0

	// 기존 MidLocation은 0.5f 지점이었다.
	// 0.65f는 중간보다 조금 더 캐릭터 쪽으로 당긴 지점이다.
	const FVector CenterCorrectionLocation = FMath::Lerp(
		TargetLocation,
		OwnerLocation,
		0.85f//0.65
	);

	// 가까울수록 보정 위치 쪽으로 당긴다.
	const float SmallTargetCenterCorrectionAlpha = DistanceAlpha * 0.85f;

	CameraLookTargetLocation = FMath::Lerp(
		TargetLocation,
		CenterCorrectionLocation,
		SmallTargetCenterCorrectionAlpha
	);
}

FVector YawDirection = CameraLookTargetLocation - OwnerLocation;
YawDirection.Z = 0.0f;

if (!YawDirection.IsNearlyZero())
{
	const FRotator TargetYawRotation = YawDirection.Rotation();
	const FRotator CurrentCameraRotation = OwnerController->GetControlRotation();

	float TargetViewHeightRatio = 0.5f;
	float PitchAlphaScale = 0.5f;

	if (bIsLargeTarget)
	{
		TargetViewHeightRatio = LockOnLargeTargetViewHeightRatio;
		PitchAlphaScale = LockOnLargeTargetPitchAlphaScale;
	}
	else
	{
		TargetViewHeightRatio = LockOnSmallTargetViewHeightRatio;
		PitchAlphaScale = LockOnSmallTargetPitchAlphaScale;
	}

	const float TargetBottomZ = TargetOrigin.Z - TargetExtent.Z;
	const float TargetViewZ = TargetBottomZ + TargetHeight * TargetViewHeightRatio;

	const FVector TargetViewLocation(
		CameraLookTargetLocation.X,
		CameraLookTargetLocation.Y,
		TargetViewZ
	);

	const FVector CameraLocation = FollowCamera->GetComponentLocation();
	const FVector PitchDirection = TargetViewLocation - CameraLocation;

	float DesiredPitch = LockOnEnterControlRotation.Pitch;

	const float PitchAlpha = FMath::Clamp(
		DistanceAlpha * PitchAlphaScale,
		0.0f,
		1.0f
	);

	if (!PitchDirection.IsNearlyZero())
	{
		const FRotator TargetLookRotation = PitchDirection.Rotation();

		const float TargetPitch = FMath::Clamp(
			TargetLookRotation.Pitch,
			-35.0f,
			35.0f
		);

		DesiredPitch = FMath::Lerp(
			LockOnEnterControlRotation.Pitch,
			TargetPitch,
			PitchAlpha
		);
	}

	const FRotator DesiredCameraRotation(
		DesiredPitch,
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
	
	/*// =========================
	// 7. 카메라 회전 보정
	// =========================
	// Yaw는 항상 타겟을 바라본다.
	// Pitch는 Large / Small 세팅에 따라 다르게 적용한다.
	FVector YawDirection = TargetLocation - OwnerLocation;
	YawDirection.Z = 0.0f;

	if (!YawDirection.IsNearlyZero())
	{
		const FRotator TargetYawRotation = YawDirection.Rotation();
		const FRotator CurrentCameraRotation = OwnerController->GetControlRotation();

		float TargetViewHeightRatio = 0.5f;
		float PitchAlphaScale = 0.5f;

		if (bIsLargeTarget)
		{
			TargetViewHeightRatio = LockOnLargeTargetViewHeightRatio;
			PitchAlphaScale = LockOnLargeTargetPitchAlphaScale;
		}
		else
		{
			TargetViewHeightRatio = LockOnSmallTargetViewHeightRatio;
			PitchAlphaScale = LockOnSmallTargetPitchAlphaScale;
		}

		const float TargetBottomZ = TargetOrigin.Z - TargetExtent.Z;
		const float TargetViewZ = TargetBottomZ + TargetHeight * TargetViewHeightRatio;

		const FVector TargetViewLocation(
			TargetOrigin.X,
			TargetOrigin.Y,
			TargetViewZ
		);

		const FVector CameraLocation = FollowCamera->GetComponentLocation();
		const FVector PitchDirection = TargetViewLocation - CameraLocation;

		float DesiredPitch = LockOnEnterControlRotation.Pitch;

		const float PitchAlpha = FMath::Clamp(
			DistanceAlpha * PitchAlphaScale,
			0.0f,
			1.0f
		);

		if (!PitchDirection.IsNearlyZero())
		{
			const FRotator TargetLookRotation = PitchDirection.Rotation();

			const float TargetPitch = FMath::Clamp(
				TargetLookRotation.Pitch,
				-35.0f,
				35.0f
			);

			DesiredPitch = FMath::Lerp(
				LockOnEnterControlRotation.Pitch,
				TargetPitch,
				PitchAlpha
			);
		}

		const FRotator DesiredCameraRotation(
			DesiredPitch,
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
	}*/

	// =========================
	// 8. 캐릭터 몸 방향 보정
	// =========================
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

	// =========================
	// Debug
	// =========================
	const FRotator ControlRotation = OwnerController->GetControlRotation();
	const FRotator BoomRotation = CameraBoom->GetComponentRotation();
}
