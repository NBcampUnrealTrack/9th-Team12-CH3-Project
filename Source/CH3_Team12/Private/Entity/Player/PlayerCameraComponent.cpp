#include "Entity/Player/PlayerCameraComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/StateTagComponent.h"
#include "Entity/Player/PlayerLocomotionComponent.h"
#include "GameplayTags/CombatGameplayTags.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Curves/CurveFloat.h"
#include "Curves/CurveVector.h"
#include "InputActionValue.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"


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

	check(StateTagComponent);

	ApplyNormalCameraInstant();
}

void UPlayerCameraComponent::Look(const FInputActionValue& Value)
{
	if (!OwnerActor)
		return;

	if (IsLockOnMode())
		return;

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

	if (APlayerController* PlayerController =
		Cast<APlayerController>(OwnerActor->GetController()))
	{
		PlayerController->SetIgnoreLookInput(true);
	}

	StateTagComponent->AddStateTag(
		CombatTags::State_Movement_LockOn
	);

	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritRoll = false;

	// 임시 처리. 나중에 Enemy는 Camera 채널 Ignore로 바꾸는 게 맞음.
	CameraBoom->bDoCollisionTest = false;

	FollowCamera->bUsePawnControlRotation = false;

	if (UCharacterMovementComponent* MovementComponent =
		OwnerActor->FindComponentByClass<UCharacterMovementComponent>())
	{
		MovementComponent->bOrientRotationToMovement = false;
		MovementComponent->bUseControllerDesiredRotation = true;
	}

	OwnerActor->bUseControllerRotationYaw = false;

	OnLockOnStateChanged.Broadcast(true);
	
	if (UPlayerLocomotionComponent* LocomotionComponent =
	OwnerActor->GetLocomotionComponent())
	{
		LocomotionComponent->RefreshMovementSettings();
	}
}

// 현재 락온 모드인지 확인
bool UPlayerCameraComponent::IsLockOnMode() const
{
	return StateTagComponent &&
		StateTagComponent->HasStateTagExact(
			CombatTags::State_Movement_LockOn
		);
}

EPlayerCameraMode UPlayerCameraComponent::GetCameraMode() const
{
	return IsLockOnMode()
		? EPlayerCameraMode::LockOn
		: EPlayerCameraMode::Normal;
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
	StartNormalCameraTransition();
}

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

void UPlayerCameraComponent::ApplyNormalCameraInstant()
{
	if (!CameraBoom || !FollowCamera)
	{
		return;
	}

	CameraBoom->TargetArmLength = NormalTargetArmLength;
	CameraBoom->SetRelativeLocation(NormalCameraBoomRelativeLocation);
	CameraBoom->SetRelativeRotation(NormalCameraBoomRelativeRotation);
	CameraBoom->SocketOffset = NormalSocketOffset;
	CameraBoom->TargetOffset = NormalTargetOffset;

	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bDoCollisionTest = false;

	FollowCamera->bUsePawnControlRotation = false;
}

void UPlayerCameraComponent::StartNormalCameraTransition()
{
	if (!OwnerActor || !CameraBoom || !FollowCamera)
	{
		return;
	}

	if (APlayerController* PlayerController =
		Cast<APlayerController>(OwnerActor->GetController()))
	{
		PlayerController->SetIgnoreLookInput(false);
	}

	if (StateTagComponent)
	{
		StateTagComponent->RemoveStateTag(
			CombatTags::State_Movement_LockOn
		);
	}

	CurrentLockOnTarget = nullptr;

	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bInheritPitch = true;
	CameraBoom->bInheritYaw = true;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bDoCollisionTest = false;

	FollowCamera->bUsePawnControlRotation = false;

	if (UCharacterMovementComponent* MovementComponent =
		OwnerActor->FindComponentByClass<UCharacterMovementComponent>())
	{
		MovementComponent->bOrientRotationToMovement = true;
		MovementComponent->bUseControllerDesiredRotation = false;
	}

	OwnerActor->bUseControllerRotationYaw = false;

	OnLockOnStateChanged.Broadcast(false);

	if (UPlayerLocomotionComponent* LocomotionComponent =
		OwnerActor->GetLocomotionComponent())
	{
		LocomotionComponent->RefreshMovementSettings();
	}
}

void UPlayerCameraComponent::UpdateLockOnCamera(float DeltaTime)
{
	if (!OwnerActor || !CurrentLockOnTarget || !CameraBoom)
	{
		ClearLockOn();
		return;
	}

	AController* OwnerController = OwnerActor->GetController();
	if (!OwnerController)
	{
		return;
	}

	const float DistanceToTarget = FVector::Dist2D(
		OwnerActor->GetActorLocation(),
		CurrentLockOnTarget->GetActorLocation()
	);

	if (DistanceToTarget > LockOnBreakDistance)
	{
		ClearLockOn();
		return;
	}

	const float DistanceAlpha = FMath::Clamp(
		(DistanceToTarget - LockOnNearDistance) /
		(LockOnFarDistance - LockOnNearDistance),
		0.0f,
		1.0f
	);

	const float MyHalfHeight =
		GetActorHalfHeight(OwnerActor);

	const float TargetHalfHeight =
		GetActorHalfHeight(CurrentLockOnTarget);

	const float HeightDifference =
		TargetHalfHeight - MyHalfHeight;

	const float DesiredArmLength = FMath::Lerp(
		LockOnCloseArmLength,
		LockOnFarArmLength,
		DistanceAlpha
	);

	float DesiredPivotHeight = FMath::Lerp(
		LockOnClosePivotHeight,
		LockOnFarPivotHeight,
		DistanceAlpha
	);

	DesiredPivotHeight += FMath::Clamp(
		HeightDifference * LockOnHeightDifferencePivotScale,
		LockOnMinHeightAdjustment,
		LockOnMaxHeightAdjustment
	);

	CameraBoom->TargetArmLength = FMath::FInterpTo(
		CameraBoom->TargetArmLength,
		DesiredArmLength,
		DeltaTime,
		LockOnCameraInterpSpeed
	);

	FVector TargetOffset = CameraBoom->TargetOffset;

	TargetOffset.Z = FMath::FInterpTo(
		TargetOffset.Z,
		DesiredPivotHeight,
		DeltaTime,
		LockOnCameraInterpSpeed
	);

	CameraBoom->TargetOffset = TargetOffset;

	const float TargetHeightRatio =
		TargetHalfHeight > MyHalfHeight * LargeTargetThreshold
			? LargeTargetFocusHeightRatio
			: NormalTargetFocusHeightRatio;

	const FVector PlayerFocus =
		GetLockOnFocusLocation(OwnerActor, PlayerFocusHeightRatio);

	const FVector TargetFocus =
		GetLockOnFocusLocation(CurrentLockOnTarget, TargetHeightRatio);

	const float FocusBias = FMath::Lerp(
		LockOnCloseFocusBias,
		LockOnFarFocusBias,
		DistanceAlpha
	);

	const FVector FocusPoint = FMath::Lerp(
		PlayerFocus,
		TargetFocus,
		FocusBias
	);

	const FVector CameraPivotLocation =
		OwnerActor->GetActorLocation() + CameraBoom->TargetOffset;

	FRotator DesiredRotation =
		UKismetMathLibrary::FindLookAtRotation(
			CameraPivotLocation,
			FocusPoint
		);

	DesiredRotation.Pitch = FMath::Clamp(
		DesiredRotation.Pitch,
		MinLockOnPitch,
		MaxLockOnPitch
	);

	DesiredRotation.Roll = 0.0f;

	const FRotator SmoothRotation = FMath::RInterpTo(
		OwnerController->GetControlRotation(),
		DesiredRotation,
		DeltaTime,
		LockOnRotationInterpSpeed
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

	TArray<FHitResult> HitResults;

	const FVector StartLocation = OwnerActor->GetActorLocation();
	const FVector EndLocation = StartLocation;

	const FCollisionShape Sphere =
		FCollisionShape::MakeSphere(LockOnTraceRadius);

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

			if (!HitActor->ActorHasTag(EnemyTagName))
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