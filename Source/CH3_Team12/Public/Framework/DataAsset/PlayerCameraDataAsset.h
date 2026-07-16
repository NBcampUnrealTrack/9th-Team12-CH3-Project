#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PlayerCameraDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FNormalCameraSettings
{
	GENERATED_BODY()

	// 기본 카메라와 캐릭터 사이의 거리.
	// 값이 커질수록 카메라가 캐릭터에서 멀어진다.
	// 값이 작아질수록 카메라가 캐릭터 쪽으로 가까워진다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TargetArmLength = 320.0f;

	// SpringArm이 캐릭터 기준으로 붙는 위치.
	// 카메라가 회전할 때 기준이 되는 피벗 위치로 사용된다.
	// Z를 올리면 카메라 기준점이 캐릭터 상체 쪽으로 올라간다.
	// Z를 내리면 카메라 기준점이 캐릭터 하체 쪽으로 내려간다.
	// Normal 카메라에서는 X/Y를 크게 조정하지 않는 편이 안정적이다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector CameraBoomRelativeLocation = FVector(0.0f, 0.0f, 70.0f);

	// SpringArm의 기본 상대 회전.
	// bUsePawnControlRotation이 true이면 컨트롤러 회전이 우선 적용된다.
	// 이 값만으로 Normal 카메라의 최종 시점을 고정한다고 생각하면 안 된다.
	// Pitch 음수는 카메라가 아래를 보는 방향이다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FRotator CameraBoomRelativeRotation = FRotator(-10.0f, 0.0f, 0.0f);

	// SpringArm 끝에서 실제 카메라 위치를 추가로 이동시키는 오프셋.
	// Z를 올리면 카메라 자체가 위로 올라간다.
	// Y를 조정하면 카메라가 좌우로 치우친 숄더 카메라 구도가 된다.
	// CameraBoomRelativeLocation은 피벗을 움직이고, SocketOffset은 카메라 위치를 움직인다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector SocketOffset = FVector(0.0f, 0.0f, 20.0f);

	// SpringArm의 월드 기준 타겟 오프셋.
	// 카메라 피벗을 월드 방향으로 밀어내는 값이다.
	// Normal 카메라에서는 값이 직관적으로 느껴지지 않을 수 있으므로 기본값 유지가 안전하다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector TargetOffset = FVector::ZeroVector;
	
	// 다른 카메라 모드에서 Normal 카메라로 돌아올 때 적용되는 보간 속도.
	// 값이 클수록 Normal 카메라 세팅으로 빠르게 복귀한다.
	// 값이 작을수록 복귀가 느리고 부드럽다.
	// 너무 크면 전환이 튀고, 너무 작으면 카메라가 늦게 따라온다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float NormalCameraInterpSpeed = 30.0f;
};

USTRUCT(BlueprintType)
struct FLockOnTraceSettings
{
	GENERATED_BODY()

	// 락온 대상을 찾는 탐색 반경.
	// 값이 커질수록 더 먼 적까지 락온 후보로 잡는다.
	// 값이 작아질수록 가까운 적만 락온 후보로 잡는다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TraceRadius = 1500.0f;

	// 락온 대상으로 인정할 Actor Tag 이름.
	// 이 태그를 가진 Actor만 락온 후보로 처리된다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName EnemyTagName = TEXT("Enemy");
};

USTRUCT(BlueprintType)
struct FLockOnCameraSettings
{
	GENERATED_BODY()

	// 락온 유지 최대 거리.
	// 플레이어와 락온 대상의 2D 거리가 이 값보다 커지면 락온이 해제된다.
	// 값이 크면 먼 거리에서도 락온이 유지된다.
	// 값이 작으면 대상이 조금만 멀어져도 락온이 풀린다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BreakDistance = 3000.0f;

	// 가까운 락온 상태로 판단하는 거리 기준.
	// 대상과의 거리가 이 값에 가까울수록 Close 계열 보정값이 강하게 적용된다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float NearDistance = 150.0f;

	// 먼 락온 상태로 판단하는 거리 기준.
	// 대상과의 거리가 이 값에 가까울수록 Far 계열 보정값이 강하게 적용된다.
	// NearDistance와 FarDistance 사이에서는 Close 값과 Far 값이 섞여 적용된다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FarDistance = 1000.0f;

	// 가까운 락온 상태에서 Normal.TargetArmLength에 더하는 거리 보정값.
	// 양수면 Normal보다 카메라가 멀어진다.
	// 음수면 Normal보다 카메라가 가까워진다.
	// 큰 적과 근접했을 때 플레이어가 화면 아래에서 잘리면 값을 올린다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CloseArmLengthOffset = 100.0f;

	// 먼 락온 상태에서 Normal.TargetArmLength에 더하는 거리 보정값.
	// 값이 클수록 먼 락온 상태에서 카메라가 더 뒤로 빠진다.
	// 플레이어와 적을 한 화면에 같이 담기 어렵다면 값을 올린다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FarArmLengthOffset = 160.0f;

	// 가까운 락온 상태에서 Normal.CameraBoomRelativeLocation.Z에 더하는 피벗 높이 보정값.
	// 값이 클수록 근접 락온 때 카메라 피벗이 위로 올라간다.
	// 위에서 아래로 내려다보는 전투 구도를 만들고 싶으면 값을 올린다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ClosePivotHeightOffset = 45.0f;

	// 먼 락온 상태에서 Normal.CameraBoomRelativeLocation.Z에 더하는 피벗 높이 보정값.
	// 값이 클수록 먼 락온 상태에서도 카메라 기준점이 위로 올라간다.
	// 먼 거리에서도 내려다보는 구도를 유지하고 싶으면 값을 올린다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FarPivotHeightOffset = 35.0f;

	// 가까운 락온 상태에서 Normal.SocketOffset.Z에 더하는 카메라 높이 보정값.
	// 양수면 카메라 자체가 더 위로 올라간다.
	// 음수면 카메라 자체가 더 아래로 내려간다.
	// 피벗 보정이 아니라 실제 카메라 위치를 직접 보정하는 값이다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CloseSocketOffsetZOffset = 0.0f;

	// 먼 락온 상태에서 Normal.SocketOffset.Z에 더하는 카메라 높이 보정값.
	// 값이 클수록 먼 락온 상태에서 카메라 자체가 더 위로 올라간다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FarSocketOffsetZOffset = 0.0f;

	// 락온 중 카메라 회전이 목표 회전으로 따라가는 속도.
	// 값이 클수록 타겟을 빠르게 바라본다.
	// 값이 작을수록 회전이 부드럽지만 반응이 느려진다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RotationInterpSpeed = 7.0f;

	// 락온 중 카메라 거리, 피벗 높이, 소켓 오프셋이 목표값으로 따라가는 속도.
	// 값이 클수록 카메라 위치 보정이 빠르게 적용된다.
	// 값이 작을수록 위치 변화가 느리고 부드럽다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CameraInterpSpeed = 6.0f;

	// 가까운 락온 상태에서 시선 중심이 적 쪽으로 치우치는 정도.
	// 0에 가까울수록 플레이어를 더 중심에 둔다.
	// 1에 가까울수록 적을 더 중심에 둔다.
	// 큰 적이 화면을 과하게 차지하면 값을 낮춘다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float CloseFocusBias = 0.35f;

	// 먼 락온 상태에서 시선 중심이 적 쪽으로 치우치는 정도.
	// 값이 클수록 먼 거리에서 적을 더 중심에 둔다.
	// 값이 작을수록 플레이어 쪽 구도가 더 유지된다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FarFocusBias = 0.50f;

	// 플레이어 Focus 지점의 높이 비율.
	// GetActorHalfHeight에 이 값을 곱한 높이를 플레이어 시선 기준점으로 사용한다.
	// 값이 낮을수록 플레이어의 하체 쪽을 본다.
	// 값이 높을수록 플레이어의 상체 쪽을 본다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PlayerFocusHeightRatio = 0.18f;

	// 일반 크기 적의 Focus 지점 높이 비율.
	// 값이 낮을수록 적의 하체나 몸통 아래쪽을 본다.
	// 값이 높을수록 적의 상체나 머리 쪽을 본다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float NormalTargetFocusHeightRatio = 0.22f;

	// 큰 적의 Focus 지점 높이 비율.
	// 큰 적을 너무 높은 위치로 바라보면 카메라가 위로 들리고 플레이어가 잘릴 수 있다.
	// 값이 낮을수록 큰 적의 몸통 아래쪽을 기준으로 바라본다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float LargeTargetFocusHeightRatio = 0.04f;

	// 적을 큰 대상으로 판단하는 키 차이 기준.
	// 대상의 HalfHeight가 플레이어 HalfHeight에 이 값을 곱한 것보다 크면 큰 적으로 처리한다.
	// 값이 낮을수록 더 많은 적이 큰 적으로 처리된다.
	// 값이 높을수록 확실히 큰 적만 큰 적으로 처리된다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float LargeTargetThreshold = 1.4f;

	// 적과 플레이어의 키 차이에 따라 카메라 피벗 높이를 추가 보정하는 비율.
	// 값이 클수록 큰 적을 락온했을 때 카메라 피벗이 더 많이 올라간다.
	// 값이 0이면 키 차이에 따른 피벗 높이 보정이 적용되지 않는다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float HeightDifferencePivotScale = 0.12f;

	// 키 차이로 인한 피벗 높이 보정의 최소값.
	// 작은 적을 락온했을 때 카메라 피벗이 과하게 내려가지 않도록 제한한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinHeightAdjustment = -10.0f;

	// 키 차이로 인한 피벗 높이 보정의 최대값.
	// 큰 적을 락온했을 때 카메라 피벗이 과하게 올라가지 않도록 제한한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxHeightAdjustment = 80.0f;

	// 락온 중 카메라 Pitch의 최소 허용값.
	// 음수 방향으로 갈수록 카메라가 더 아래를 볼 수 있다.
	// 이 값은 아래를 보게 강제하는 값이 아니라, 아래로 볼 수 있는 한계를 정하는 값이다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinPitch = -60.0f;

	// 락온 중 카메라 Pitch의 최대 허용값.
	// 값이 커질수록 카메라가 위쪽을 더 볼 수 있다.
	// 락온 중 카메라가 위로 들리는 느낌이 강하면 값을 낮춘다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxPitch = 0.0f;
	
	// 락온 중 계산된 카메라 Pitch에 추가로 더하는 보정값.
	// 음수면 카메라가 계산된 시점보다 더 아래를 본다.
	// 양수면 카메라가 계산된 시점보다 더 위를 본다.
	// 락온 중 플레이어 하체가 잘리거나 바닥을 더 보고 싶으면 음수 값을 더 크게 준다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PitchOffset = -5.0f;
};

USTRUCT(BlueprintType)
struct FCameraCollisionSettings
{
	GENERATED_BODY()

	// SpringArm 카메라 충돌 사용 여부.
	// true면 벽이나 지형에 카메라가 파묻히지 않도록 SpringArm 길이가 자동으로 줄어든다.
	// false면 카메라가 벽이나 지형을 뚫고 들어갈 수 있다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bDoCollisionTest = true;

	// 카메라 충돌 감지 구체의 크기.
	// 값이 클수록 벽과 지형에 더 일찍 반응한다.
	// 값이 작을수록 카메라가 벽에 더 가까이 붙는다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ProbeSize = 12.0f;

	// 카메라 충돌을 검사할 Collision Channel.
	// 이 채널을 Block하는 오브젝트는 카메라를 밀어낸다.
	// 플레이어, 적, 무기, 히트박스는 이 채널을 Ignore하는 편이 좋다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TEnumAsByte<ECollisionChannel> ProbeChannel = ECC_Camera;
};

USTRUCT(BlueprintType)
struct FExecutionCameraSettings
{
	GENERATED_BODY()

	// 처형 카메라에서 플레이어와 타겟을 잇는 방향 기준으로 오른쪽으로 이동하는 거리.
	// 값이 클수록 카메라가 캐릭터 오른쪽 측면으로 더 이동한다.
	// 오른손 무기 액션이나 측면 처형 구도를 강조할 때 사용한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RightOffset = 120.0f;

	// 처형 카메라에서 플레이어와 타겟을 잇는 방향 기준으로 뒤로 빠지는 거리.
	// 값이 클수록 카메라가 뒤로 물러난다.
	// 값이 작을수록 처형 장면이 더 근접하게 보인다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BackOffset = 220.0f;

	// 처형 카메라의 월드 기준 높이.
	// 값이 클수록 카메라가 위로 올라간다.
	// 너무 높으면 내려다보는 느낌이 강해지고, 너무 낮으면 캐릭터 몸에 시야가 가릴 수 있다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float UpOffset = 0.0f;

	// 처형 카메라가 플레이어를 바라볼 때 사용하는 기준 높이.
	// 값이 높을수록 플레이어 상체 쪽을 바라본다.
	// 값이 낮을수록 플레이어 몸통 아래쪽을 바라본다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float PlayerFocusHeight = 80.0f;

	// 처형 카메라가 타겟을 바라볼 때 사용하는 기준 높이.
	// 값이 높을수록 타겟 상체 쪽을 바라본다.
	// 값이 낮을수록 타겟 몸통 아래쪽을 바라본다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float TargetFocusHeight = 90.0f;

	// 처형 카메라의 시선 중심이 플레이어와 타겟 사이에서 어디에 놓이는지 결정한다.
	// 0에 가까울수록 플레이어를 중심으로 바라본다.
	// 1에 가까울수록 타겟을 중심으로 바라본다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float FocusBias = 0.60f;

	// 처형 중 카메라 위치가 목표 위치로 이동하는 속도.
	// 값이 클수록 목표 위치에 빠르게 도달한다.
	// 값이 작을수록 이동이 느리고 부드럽다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float LocationInterpSpeed = 14.0f;

	// 처형 중 카메라 회전이 목표 회전으로 이동하는 속도.
	// 값이 클수록 빠르게 목표 방향을 바라본다.
	// 값이 작을수록 회전이 느리고 부드럽다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float RotationInterpSpeed = 14.0f;

	// 처형 카메라 Pitch의 최소 허용값.
	// 음수 방향으로 갈수록 카메라가 더 아래를 볼 수 있다.
	// 너무 낮게 설정하면 처형 중 바닥을 과하게 볼 수 있다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MinPitch = -35.0f;

	// 처형 카메라 Pitch의 최대 허용값.
	// 값이 커질수록 카메라가 위쪽을 더 볼 수 있다.
	// 너무 높게 설정하면 처형 중 카메라가 위로 들릴 수 있다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MaxPitch = 10.0f;

	// 처형 카메라 시작 시 목표 위치로 즉시 이동할지 결정한다.
	// true이면 처형 시작 순간 연출 카메라 위치로 바로 이동한다.
	// false이면 현재 카메라 위치에서 목표 위치까지 보간 이동한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bSnapOnStart = true;
};

UCLASS(BlueprintType)
class CH3_TEAM12_API UPlayerCameraDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// 일반 이동과 탐색 상태에서 사용하는 기본 카메라 세팅.
	// LockOn 카메라는 이 Normal 값을 기준으로 추가 보정을 적용한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera|Normal")
	FNormalCameraSettings Normal;

	// 락온 대상 탐색에 사용하는 세팅.
	// 카메라 구도가 아니라 어떤 Actor를 락온 후보로 잡을지 결정한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera|LockOn|Trace")
	FLockOnTraceSettings Trace;

	// 락온 중 사용하는 카메라 세팅.
	// Normal 카메라를 기준으로 거리, 높이, 시선 중심, Pitch를 보정한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera|LockOn")
	FLockOnCameraSettings LockOn;
	
	// SpringArm 카메라 충돌 세팅.
	// 벽과 지형에 카메라가 파묻히지 않도록 조정한다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera|Collision")
	FCameraCollisionSettings Collision;
	
	// 처형 연출에서 사용하는 특수 카메라 세팅.
	// 일반 추적 카메라가 아니라 월드 위치와 시선 중심을 직접 계산하는 연출용 세팅이다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Camera|Execution")
	FExecutionCameraSettings Execution;
};