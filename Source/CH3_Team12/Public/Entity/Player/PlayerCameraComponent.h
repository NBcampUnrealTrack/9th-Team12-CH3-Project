#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCameraComponent.generated.h"

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
	
	void SetupNormalCamera();
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
	float NormalTargetArmLength = 300.0f;

	UPROPERTY(EditAnywhere, Category = "Camera|Normal")
	bool bNormalUsePawnControlRotation = true;

	UPROPERTY(EditAnywhere, Category = "Camera|Normal")
	FVector NormalCameraBoomRelativeLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category = "Camera|Normal")
	FVector NormalSocketOffset = FVector(0.0f, 0.0f, 30.0f);

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


	// =========================
	// 거리 기준
	// =========================
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|Distance")
	float LockOnHeightCorrectionNearDistance = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|Distance")
	float LockOnHeightCorrectionFarDistance = 600.0f;


	// =========================
	// 크기 판단
	// =========================
	// 키 차이 허용값
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|Height")
	float LockOnTargetHeightDeadZone = 60.0f;


	// =========================
	// 큰 타겟 보정
	// =========================
	// 멀리 있을 때 카메라 높이 보정
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|LargeTarget")
	float LockOnLargeTargetFarSocketZOffset = 0.0f;
	// 가까이 있을 때 카메라 높이 보정
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|LargeTarget")
	float LockOnLargeTargetCloseSocketZOffset = -15.0f;
	// 가까이 있을 때 스프링암 시작 위치 보정
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|LargeTarget")
	FVector LockOnLargeTargetCloseRelativeLocation = FVector(0.0f, 0.0f, 10.0f);

	// 타겟을 바라볼 높이 비율
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|LargeTarget")
	float LockOnLargeTargetViewHeightRatio = 0.60f;
	
	// Pitch 보정 강도
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|LargeTarget")
	float LockOnLargeTargetPitchAlphaScale = 0.9f;
	
	// 큰 보스 락온 시 거리에 따른 스프링암 길이
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|LargeTarget")
	float LockOnLargeTargetArmLengthFar = 350.0f;

	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|LargeTarget")
	float LockOnLargeTargetArmLengthClose = 280.0f;
	
	
	// =========================
	// 작은 타겟 보정
	// =========================
	// 멀리 있을 때 카메라 높이 보정
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|SmallTarget")
	float LockOnSmallTargetFarSocketZOffset = 0.0f;
	// 가까이 있을 때 카메라 높이 보정
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|SmallTarget")
	float LockOnSmallTargetCloseSocketZOffset = 0.0f;
	// 가까이 있을 때 스프링암 시작 위치 보정
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|SmallTarget")
	FVector LockOnSmallTargetCloseRelativeLocation = FVector(0.0f, 0.0f, 0.0f);

	// 타겟을 바라볼 높이 비율
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|SmallTarget")
	float LockOnSmallTargetViewHeightRatio = 0.35f;
	// Pitch 보정 강도
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|SmallTarget")
	float LockOnSmallTargetPitchAlphaScale = 0.8f;
	
	// 작은 몬스터 락온 시 거리에 따른 스프링암 길이.
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|SmallTarget")
	float LockOnSmallTargetArmLengthFar = 350.0f;
	UPROPERTY(EditAnywhere, Category = "Camera|LockOn|SmallTarget")
	float LockOnSmallTargetArmLengthClose = 300.0f;
};