#include "Entity/Player/PlayerCameraComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/StateTagComponent.h"
#include "Entity/Player/PlayerLocomotionComponent.h"
#include "GameplayTags/CombatGameplayTags.h"
#include "Framework/DataAsset/PlayerCameraDataAsset.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "InputActionValue.h"
#include "Kismet/KismetMathLibrary.h"

UPlayerCameraComponent::UPlayerCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerActor = Cast<APlayerCharacterBase>(GetOwner());

	if (!OwnerActor)
	{
		return;
	}

	CameraBoom = OwnerActor->GetCameraBoom();
	FollowCamera = OwnerActor->GetFollowCamera();
	StateTagComponent = OwnerActor->GetStateTagComponent();

	if (!CameraBoom || !FollowCamera || !StateTagComponent || !CameraData)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCameraComponent: Required reference is missing"));
		return;
	}

	ApplyNormalCameraInstant();
}

void UPlayerCameraComponent::Look(const FInputActionValue& Value)
{
	if (!OwnerActor)
		return;

	if (CameraMode != EPlayerCameraMode::Normal)
	{
		return;
	}

	const FVector2D LookInput = Value.Get<FVector2D>();

	OwnerActor->AddControllerYawInput(LookInput.X);
	OwnerActor->AddControllerPitchInput(LookInput.Y);
}

void UPlayerCameraComponent::SetupLockOnCamera()
{
	if (!OwnerActor || !CameraBoom || !FollowCamera || !CurrentLockOnTarget)
	{
		return;
	}

	CameraMode = EPlayerCameraMode::LockOn;
	
	StateTagComponent->AddStateTag(
		CombatTags::State_Movement_LockOn
	);

	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritRoll = false;

	ApplyCameraCollisionSettings();

	FollowCamera->bUsePawnControlRotation = false;

	if (UCharacterMovementComponent* MovementComponent =
		OwnerActor->FindComponentByClass<UCharacterMovementComponent>())
	{
		MovementComponent->bOrientRotationToMovement = false;
		MovementComponent->bUseControllerDesiredRotation = true;
	}

	OwnerActor->bUseControllerRotationYaw = false;

	OnLockOnStateChanged.Broadcast(
		true,
		CurrentLockOnTarget
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("LockOn ON: %s"),
		*GetNameSafe(CurrentLockOnTarget)
	);

	if (UPlayerLocomotionComponent* LocomotionComponent =
		OwnerActor->GetLocomotionComponent())
	{
		LocomotionComponent->RefreshMovementSettings();
	}
}

// 현재 락온 모드인지 확인
bool UPlayerCameraComponent::IsLockOn() const
{
	return StateTagComponent &&
		StateTagComponent->HasStateTagExact(CombatTags::State_Movement_LockOn);
}

void UPlayerCameraComponent::StartExecutionCamera(AActor* ExecutionTarget)
{
	if (!OwnerActor || !CameraBoom || !FollowCamera || !CameraData || !ExecutionTarget)
	{
		return;
	}

	CurrentExecutionTarget = ExecutionTarget;

	if (IsLockOn())
	{
		CurrentLockOnTarget = ExecutionTarget;
	}
	
	CameraMode = EPlayerCameraMode::Execution;

	CameraBoom->SetAbsolute(true, false, false);

	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritRoll = false;

	CameraBoom->TargetArmLength = 0.0f;
	CameraBoom->SocketOffset = FVector::ZeroVector;
	CameraBoom->TargetOffset = FVector::ZeroVector;

	CameraBoom->bEnableCameraLag = false;
	CameraBoom->bEnableCameraRotationLag = false;
	CameraBoom->bDoCollisionTest = false;

	FollowCamera->bUsePawnControlRotation = false;

	if (CameraData->Execution.bSnapOnStart)
	{
		ApplyExecutionCamera(0.0f, true);
	}
}

void UPlayerCameraComponent::EndExecutionCamera()
{
	if (CameraMode != EPlayerCameraMode::Execution)
	{
		return;
	}

	CurrentExecutionTarget = nullptr;

	SetupLockOnCamera();
	
	// PrepareNormalCameraTransitionFromExecution();

	// StartNormalCameraTransition();
}

// 락온 상태면 해제, 아니면 락온 시도
void UPlayerCameraComponent::LockOn()
{
	if (IsLockOn())
	{
		ClearLockOn();
		return;
	}

	TryLockOn();
}

// 락온 대상을 비우고 노말 카메라로 복귀
void UPlayerCameraComponent::ClearLockOn()
{
	StartNormalCameraTransition();
}

void UPlayerCameraComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	switch (CameraMode)
	{
	case EPlayerCameraMode::Execution:
		UpdateExecutionCamera(DeltaTime);
		break;

	case EPlayerCameraMode::LockOn:
		UpdateLockOnCamera(DeltaTime);
		break;

	case EPlayerCameraMode::Normal:
	default:
		UpdateNormalCamera(DeltaTime);
		break;
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
	
	const FNormalCameraSettings& Normal = CameraData->Normal;
	const float InterpSpeed = Normal.NormalCameraInterpSpeed;

	CameraBoom->SetRelativeLocation(
		FMath::VInterpTo(
			CameraBoom->GetRelativeLocation(),
			Normal.CameraBoomRelativeLocation,
			DeltaTime,
			InterpSpeed
		)
	);

	CameraBoom->TargetArmLength = FMath::FInterpTo(
		CameraBoom->TargetArmLength,
		Normal.TargetArmLength,
		DeltaTime,
		InterpSpeed
	);

	CameraBoom->SocketOffset = FMath::VInterpTo(
		CameraBoom->SocketOffset,
		Normal.SocketOffset,
		DeltaTime,
		InterpSpeed
	);

	CameraBoom->TargetOffset = FMath::VInterpTo(
		CameraBoom->TargetOffset,
		Normal.TargetOffset,
		DeltaTime,
		InterpSpeed
	);
	
	const bool bAlmostNormal =
	CameraBoom->GetRelativeLocation().Equals(
		Normal.CameraBoomRelativeLocation,
		3.0f
	) &&
	FMath::IsNearlyEqual(
		CameraBoom->TargetArmLength,
		Normal.TargetArmLength,
		3.0f
	);

	if (bAlmostNormal)
	{
		CameraBoom->bEnableCameraLag = true;
		CameraBoom->bEnableCameraRotationLag = true;
	}
}

void UPlayerCameraComponent::UpdateExecutionCamera(float DeltaTime)
{
	ApplyExecutionCamera(DeltaTime, false);
}

void UPlayerCameraComponent::ApplyExecutionCamera(
	float DeltaTime,
	bool bInstant)
{
	if (!OwnerActor || !CameraBoom || !CameraData)
	{
		return;
	}

	if (!IsValid(CurrentExecutionTarget))
	{
		return;
	}
	
	AController* OwnerController = OwnerActor->GetController();

	if (!OwnerController)
	{
		return;
	}

	const FExecutionCameraSettings& Execution = CameraData->Execution;

	const FVector PlayerLocation =
		OwnerActor->GetActorLocation();

	const FVector TargetLocation =
		CurrentExecutionTarget->GetActorLocation();

	FVector ToTarget = TargetLocation - PlayerLocation;
	ToTarget.Z = 0.0f;

	if (!ToTarget.Normalize())
	{
		ToTarget = OwnerActor->GetActorForwardVector();
		ToTarget.Z = 0.0f;
		ToTarget.Normalize();
	}

	// 플레이어-보스 라인을 기준으로 한 오른쪽 방향.
	// 플레이어가 보스를 바라보게 정렬되어 있다면 ActorRightVector와 거의 같다.
	const FVector RightDirection =
		FVector::CrossProduct(FVector::UpVector, ToTarget).GetSafeNormal();

	const FVector DesiredCameraLocation =
		PlayerLocation
		+ RightDirection * Execution.RightOffset
		- ToTarget * Execution.BackOffset
		+ FVector::UpVector * Execution.UpOffset;

	const FVector PlayerFocus =
		PlayerLocation + FVector::UpVector * Execution.PlayerFocusHeight;

	const FVector TargetFocus =
		TargetLocation + FVector::UpVector * Execution.TargetFocusHeight;

	const FVector FocusPoint =
		FMath::Lerp(PlayerFocus, TargetFocus, Execution.FocusBias);

	FRotator DesiredRotation =
		UKismetMathLibrary::FindLookAtRotation(
			DesiredCameraLocation,
			FocusPoint
		);

	DesiredRotation.Pitch = FMath::Clamp(
		DesiredRotation.Pitch,
		Execution.MinPitch,
		Execution.MaxPitch
	);

	DesiredRotation.Roll = 0.0f;

	if (bInstant)
	{
		CameraBoom->SetWorldLocation(DesiredCameraLocation);
		OwnerController->SetControlRotation(DesiredRotation);
		return;
	}

	const FVector SmoothCameraLocation =
		FMath::VInterpTo(
			CameraBoom->GetComponentLocation(),
			DesiredCameraLocation,
			DeltaTime,
			Execution.LocationInterpSpeed
		);

	const FRotator SmoothRotation =
		FMath::RInterpTo(
			OwnerController->GetControlRotation(),
			DesiredRotation,
			DeltaTime,
			Execution.RotationInterpSpeed
		);

	CameraBoom->SetWorldLocation(SmoothCameraLocation);
	OwnerController->SetControlRotation(SmoothRotation);
}

void UPlayerCameraComponent::ApplyNormalCameraInstant()
{
	if (!CameraBoom || !FollowCamera)
	{
		return;
	}

	
	const FNormalCameraSettings& Normal = CameraData->Normal;

	CameraBoom->TargetArmLength = Normal.TargetArmLength;
	CameraBoom->SetRelativeLocation(Normal.CameraBoomRelativeLocation);
	CameraBoom->SetRelativeRotation(Normal.CameraBoomRelativeRotation);
	CameraBoom->SocketOffset = Normal.SocketOffset;
	CameraBoom->TargetOffset = Normal.TargetOffset;
	CameraBoom->CameraLagSpeed = 4.0f;
	CameraBoom->CameraLagMaxDistance = 200.0f;
	CameraBoom->CameraRotationLagSpeed = 12.0f;
	CameraBoom->SetAbsolute(false, false, false);
	
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bEnableCameraRotationLag = true;

	ApplyCameraCollisionSettings();
	
	FollowCamera->bUsePawnControlRotation = false;
}

void UPlayerCameraComponent::StartNormalCameraTransition()
{
	if (!OwnerActor || !CameraBoom || !FollowCamera)
	{
		return;
	}

	const bool bWasLockOn =
		IsLockOn() || IsValid(CurrentLockOnTarget);

	AActor* PreviousLockOnTarget = CurrentLockOnTarget;

	CameraMode = EPlayerCameraMode::Normal;

	CameraBoom->SetAbsolute(false, false, false);

	if (StateTagComponent)
	{
		StateTagComponent->RemoveStateTag(
			CombatTags::State_Movement_LockOn
		);
	}

	CurrentLockOnTarget = nullptr;

	if (bWasLockOn)
	{
		OnLockOnStateChanged.Broadcast(
			false,
			PreviousLockOnTarget
		);

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("LockOn OFF: %s"),
			*GetNameSafe(PreviousLockOnTarget)
		);
	}

	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritRoll = false;

	CameraBoom->bEnableCameraLag = false;
	CameraBoom->bEnableCameraRotationLag = false;

	ApplyCameraCollisionSettings();

	FollowCamera->bUsePawnControlRotation = false;

	if (UCharacterMovementComponent* MovementComponent =
		OwnerActor->FindComponentByClass<UCharacterMovementComponent>())
	{
		MovementComponent->bOrientRotationToMovement = true;
		MovementComponent->bUseControllerDesiredRotation = false;
	}

	OwnerActor->bUseControllerRotationYaw = false;

	if (UPlayerLocomotionComponent* LocomotionComponent =
		OwnerActor->GetLocomotionComponent())
	{
		LocomotionComponent->RefreshMovementSettings();
	}
}

void UPlayerCameraComponent::UpdateLockOnCamera(float DeltaTime)
{
	if (!OwnerActor || !IsValid(CurrentLockOnTarget) || !CameraBoom || !CameraData)
	{
		ClearLockOn();
		return;
	}

	const FNormalCameraSettings& Normal = CameraData->Normal;
	const FLockOnCameraSettings& LockOn = CameraData->LockOn;

	AController* OwnerController = OwnerActor->GetController();
	if (!OwnerController)
	{
		return;
	}

	const float DistanceToTarget = FVector::Dist2D(
		OwnerActor->GetActorLocation(),
		CurrentLockOnTarget->GetActorLocation()
	);

	if (DistanceToTarget > LockOn.BreakDistance)
	{
		ClearLockOn();
		return;
	}

	const float DistanceRange =
		FMath::Max(
			LockOn.FarDistance - LockOn.NearDistance,
			1.0f
		);

	const float DistanceAlpha = FMath::Clamp(
		(DistanceToTarget - LockOn.NearDistance) / DistanceRange,
		0.0f,
		1.0f
	);

	const float MyHalfHeight =
		GetActorHalfHeight(OwnerActor);

	const float TargetHalfHeight =
		GetActorHalfHeight(CurrentLockOnTarget);

	const float HeightDifference =
		TargetHalfHeight - MyHalfHeight;

	const float HeightAdjustment =
		FMath::Clamp(
			HeightDifference * LockOn.HeightDifferencePivotScale,
			LockOn.MinHeightAdjustment,
			LockOn.MaxHeightAdjustment
		);

	const float DesiredArmLength =
		Normal.TargetArmLength +
		FMath::Lerp(
			LockOn.CloseArmLengthOffset,
			LockOn.FarArmLengthOffset,
			DistanceAlpha
		);

	FVector DesiredBoomLocation =
		Normal.CameraBoomRelativeLocation;

	DesiredBoomLocation.Z +=
		FMath::Lerp(
			LockOn.ClosePivotHeightOffset,
			LockOn.FarPivotHeightOffset,
			DistanceAlpha
		) + HeightAdjustment;

	FVector DesiredSocketOffset =
		Normal.SocketOffset;

	DesiredSocketOffset.Z +=
		FMath::Lerp(
			LockOn.CloseSocketOffsetZOffset,
			LockOn.FarSocketOffsetZOffset,
			DistanceAlpha
		);

	const FVector DesiredTargetOffset =
		Normal.TargetOffset;

	CameraBoom->TargetArmLength =
		FMath::FInterpTo(
			CameraBoom->TargetArmLength,
			DesiredArmLength,
			DeltaTime,
			LockOn.CameraInterpSpeed
		);

	CameraBoom->SetRelativeLocation(
		FMath::VInterpTo(
			CameraBoom->GetRelativeLocation(),
			DesiredBoomLocation,
			DeltaTime,
			LockOn.CameraInterpSpeed
		)
	);

	CameraBoom->SocketOffset =
		FMath::VInterpTo(
			CameraBoom->SocketOffset,
			DesiredSocketOffset,
			DeltaTime,
			LockOn.CameraInterpSpeed
		);

	CameraBoom->TargetOffset =
		FMath::VInterpTo(
			CameraBoom->TargetOffset,
			DesiredTargetOffset,
			DeltaTime,
			LockOn.CameraInterpSpeed
		);

	const float TargetHeightRatio =
		TargetHalfHeight > MyHalfHeight * LockOn.LargeTargetThreshold
			? LockOn.LargeTargetFocusHeightRatio
			: LockOn.NormalTargetFocusHeightRatio;

	const FVector PlayerFocus =
		GetLockOnFocusLocation(
			OwnerActor,
			LockOn.PlayerFocusHeightRatio
		);

	const FVector TargetFocus =
		GetLockOnFocusLocation(
			CurrentLockOnTarget,
			TargetHeightRatio
		);

	const float FocusBias =
		FMath::Lerp(
			LockOn.CloseFocusBias,
			LockOn.FarFocusBias,
			DistanceAlpha
		);

	const FVector FocusPoint =
		FMath::Lerp(
			PlayerFocus,
			TargetFocus,
			FocusBias
		);

	const FVector CameraPivotLocation =
		CameraBoom->GetComponentLocation();

	FRotator DesiredRotation =
		UKismetMathLibrary::FindLookAtRotation(
			CameraPivotLocation,
			FocusPoint
		);

	DesiredRotation.Pitch += LockOn.PitchOffset;

	DesiredRotation.Pitch = FMath::Clamp(
		DesiredRotation.Pitch,
		LockOn.MinPitch,
		LockOn.MaxPitch
	);
	
	DesiredRotation.Roll = 0.0f;

	const FRotator SmoothRotation =
		FMath::RInterpTo(
			OwnerController->GetControlRotation(),
			DesiredRotation,
			DeltaTime,
			LockOn.RotationInterpSpeed
		);

	OwnerController->SetControlRotation(SmoothRotation);
}

AActor* UPlayerCameraComponent::FindLockOnTarget() const
{
	if (!OwnerActor)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	const FLockOnTraceSettings& Trace = CameraData->Trace;

	TArray<FHitResult> HitResults;

	const FVector StartLocation = OwnerActor->GetActorLocation();
	const FVector EndLocation = StartLocation;

	const FCollisionShape Sphere =
		FCollisionShape::MakeSphere(Trace.TraceRadius);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwnerActor);

	const bool bHit = World->SweepMultiByChannel(
		HitResults,
		StartLocation,
		EndLocation,
		FQuat::Identity,
		ECC_Pawn,
		Sphere,
		QueryParams
	);

	AActor* ClosestEnemy = nullptr;
	float MinDistanceSq = TNumericLimits<float>::Max();

	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();

			if (!HitActor)
			{
				continue;
			}

			if (!HitActor->ActorHasTag(Trace.EnemyTagName))
			{
				continue;
			}

			const float DistanceSq = FVector::DistSquared(
				StartLocation,
				HitActor->GetActorLocation()
			);

			if (DistanceSq < MinDistanceSq)
			{
				MinDistanceSq = DistanceSq;
				ClosestEnemy = HitActor;
			}
		}
	}

	return ClosestEnemy;
}

void UPlayerCameraComponent::TryLockOn()
{
	if (!OwnerActor)
	{
		return;
	}
	
	if (!StateTagComponent->HasStateTagExact(CombatTags::State_Combat_Armed))
	{
		return;
	}

	CurrentLockOnTarget = FindLockOnTarget();

	if (!CurrentLockOnTarget)
	{
		UE_LOG(LogTemp, Warning, TEXT("No LockOn Target"));
		return;
	}

	SetupLockOnCamera();
}

float UPlayerCameraComponent::GetActorHalfHeight(
	AActor* Actor) const
{
	if (!Actor)
	{
		return 88.0f;
	}

	if (ACharacter* Character = Cast<ACharacter>(Actor))
	{
		if (UCapsuleComponent* Capsule =
			Character->GetCapsuleComponent())
		{
			return Capsule->GetScaledCapsuleHalfHeight();
		}
	}

	FVector Origin;
	FVector Extent;
	Actor->GetActorBounds(false, Origin, Extent);

	return Extent.Z;
}

FVector UPlayerCameraComponent::GetLockOnFocusLocation(
	AActor* Actor,
	float HeightRatio) const
{
	if (!Actor)
	{
		return FVector::ZeroVector;
	}

	const float HalfHeight = GetActorHalfHeight(Actor);

	return Actor->GetActorLocation()
		+ FVector(0.0f, 0.0f, HalfHeight * HeightRatio);
}

void UPlayerCameraComponent::ApplyCameraCollisionSettings()
{
	if (!CameraBoom || !CameraData)
	{
		return;
	}

	CameraBoom->bDoCollisionTest =
		CameraData->Collision.bDoCollisionTest;

	CameraBoom->ProbeSize =
		CameraData->Collision.ProbeSize;

	CameraBoom->ProbeChannel =
		CameraData->Collision.ProbeChannel;
}

void UPlayerCameraComponent::PrepareNormalCameraTransitionFromExecution()
{
	if (!CameraBoom || !FollowCamera || !CameraData)
	{
		return;
	}

	const FVector CurrentWorldLocation =
		CameraBoom->GetComponentLocation();

	CameraBoom->SetAbsolute(false, false, false);

	if (USceneComponent* Parent = CameraBoom->GetAttachParent())
	{
		const FVector RelativeLocation =
			Parent->GetComponentTransform()
			.InverseTransformPosition(CurrentWorldLocation);

		CameraBoom->SetRelativeLocation(RelativeLocation);
	}
	else
	{
		CameraBoom->SetWorldLocation(CurrentWorldLocation);
	}

	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritRoll = false;

	CameraBoom->TargetArmLength = 0.0f;
	CameraBoom->SocketOffset = FVector::ZeroVector;
	CameraBoom->TargetOffset = FVector::ZeroVector;

	// 전환 중 이중 보간/버벅임 방지
	CameraBoom->bEnableCameraLag = false;
	CameraBoom->bEnableCameraRotationLag = false;

	ApplyCameraCollisionSettings();

	FollowCamera->bUsePawnControlRotation = false;
}