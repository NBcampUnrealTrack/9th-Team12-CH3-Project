#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCameraComponent.generated.h"

struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnLockOnStateChanged,
	bool,
	bIsLockOn
);

UENUM(BlueprintType)
enum class EPlayerCameraMode : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	LockOn UMETA(DisplayName = "LockOn")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH3_TEAM12_API UPlayerCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerCameraComponent();
	
public:
	virtual void BeginPlay() override;
	
	void Look(const FInputActionValue& Value);
	AActor* GetCurrentLockOnTarget() const { return CurrentLockOnTarget; }
	
	void SetupLockOnCamera();
	
	void LockOn();
	void TryLockOn();
	void ClearLockOn();
	
	bool IsLockOnMode() const;
	
	EPlayerCameraMode GetCameraMode() const;
	
protected:
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	
public:
	// 상태 델리게이트 
	UPROPERTY(BlueprintAssignable, Category="Camera|LockOn")
	FOnLockOnStateChanged OnLockOnStateChanged;
	
private:
	void UpdateLockOnCamera(float DeltaTime);
	void UpdateNormalCamera(float DeltaTime);
	
	void ApplyNormalCameraInstant();
	void StartNormalCameraTransition();
	
private:
	UPROPERTY()
	TObjectPtr<class APlayerCharacterBase> OwnerActor;

	UPROPERTY()
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY()
	TObjectPtr<class UCameraComponent> FollowCamera;

	UPROPERTY()
	TObjectPtr<class AActor> CurrentLockOnTarget;
	
	UPROPERTY()
	TObjectPtr<class UStateTagComponent> StateTagComponent;


	// =========================
	// 상태
	// =========================

	// 락온 진입 시 회전값
	// 락온 중 멀어질 때 Pitch를 원래 노말 시점으로 되돌릴 때 사용한다.
	UPROPERTY()
	FRotator LockOnEnterControlRotation = FRotator::ZeroRotator;


	// =========================
	// 노말 카메라
	// =========================
	UPROPERTY(EditAnywhere, Category = "Camera|Normal")
	float NormalTargetArmLength = 350.0f;

	UPROPERTY(EditAnywhere, Category = "Camera|Normal")
	bool bNormalUsePawnControlRotation = true;

	UPROPERTY(EditAnywhere, Category = "Camera|Normal")
	FVector NormalCameraBoomRelativeLocation = FVector(0.0f, 0.0f, 50.0f);

	UPROPERTY(EditAnywhere, Category = "Camera|Normal")
	FRotator NormalCameraBoomRelativeRotation = FRotator(-10.0f, 0.0f, 0.0f);

	UPROPERTY(EditAnywhere, Category = "Camera|Normal")
	FVector NormalSocketOffset = FVector(0.0f, 0.0f, 40.0f);

	UPROPERTY(EditAnywhere, Category = "Camera|Normal")
	FVector NormalTargetOffset = FVector::ZeroVector;


	// =========================
	// 락온 탐색
	// =========================
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|Trace")
	float LockOnTraceDistance = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|Trace")
	float LockOnTraceRadius = 1500.0f;

	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|Trace")
	float LockOnTraceHalfHeight = 1500.0f;

	// 적에 대한 네임태그
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|Trace")
	FName EnemyTagName = TEXT("Enemy");

	// =========================
	// 보간
	// =========================
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|Interp")
	float LockOnCameraRotationInterpSpeed = 6.0f;

	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|Interp")
	float LockOnBodyRotationInterpSpeed = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Camera|Interp")
	float CameraInterpSpeed = 6.0f;

	// 데이터 커브를 사용한 카메라 전환
	// X = LocationZ, Y = RotationY, Z = SocketZ
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|Curve|CameraOffset")
	TObjectPtr<class UCurveVector> LockOnSmallMonsterCameraOffsetByDistanceCurve;

	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|Curve|CameraOffset")
	TObjectPtr<class UCurveVector> LockOnMediumMonsterCameraOffsetByDistanceCurve;

	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|Curve|CameraOffset")
	TObjectPtr<class UCurveVector> LockOnLargeMonsterCameraOffsetByDistanceCurve;

	// 암길이 전용
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|Curve|ArmLength")
	TObjectPtr<class UCurveFloat> LockOnSmallMonsterArmLengthByDistanceCurve;

	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|Curve|ArmLength")
	TObjectPtr<class UCurveFloat> LockOnMediumMonsterArmLengthByDistanceCurve;

	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|Curve|ArmLength")
	TObjectPtr<class UCurveFloat> LockOnLargeMonsterArmLengthByDistanceCurve;

	// 키 차이 허용 범위
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|TargetSize")
	float LockOnTargetHeightDiff = 30.0f;
	
	
private:
	AActor* FindLockOnTarget() const;

	FVector GetLockOnFocusLocation(
		AActor* Actor,
		float HeightRatio
	) const;

	float GetActorHalfHeight(AActor* Actor) const;

private:
	UPROPERTY(EditAnywhere, Category="Camera|LockOn")
	float LockOnBreakDistance = 1500.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn")
	float LockOnNearDistance = 100.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn")
	float LockOnFarDistance = 1000.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn")
	float LockOnCloseArmLength = 500.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn")
	float LockOnFarArmLength = 700.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn")
	float LockOnClosePivotHeight = 90.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn")
	float LockOnFarPivotHeight = 50.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn")
	float LockOnRotationInterpSpeed = 7.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn")
	float LockOnCameraInterpSpeed = 6.0f;
	
	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Focus")
	float LockOnCloseFocusBias = 0.45f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Focus")
	float LockOnFarFocusBias = 0.60f;
	
	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Height")
	float LockOnHeightDifferencePivotScale = 0.15f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Height")
	float LockOnMinHeightAdjustment = -20.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Height")
	float LockOnMaxHeightAdjustment = 60.0f;
	
	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Focus")
	float PlayerFocusHeightRatio = 0.25f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Focus")
	float NormalTargetFocusHeightRatio = 0.35f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Focus")
	float LargeTargetFocusHeightRatio = 0.10f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Focus")
	float LargeTargetThreshold = 1.4f;
	
	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Rotation")
	float MinLockOnPitch = -45.0f;

	UPROPERTY(EditAnywhere, Category="Camera|LockOn|Rotation")
	float MaxLockOnPitch = 5.0f;
};