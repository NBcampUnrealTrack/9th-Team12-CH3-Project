#include "Entity/Player/PlayerTimeWarpComponent.h"

#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerDefenseComponent.h"
#include "Entity/Player/StateTagComponent.h"
#include "Framework/DataAsset/PlayerTimeWarpDataAsset.h"
#include "GameplayTags/CombatGameplayTags.h"
#include "Entity/Player/PlayerCameraComponent.h"

#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Framework/Subsystem/KatanaSoundManagerSubsystem.h"

UPlayerTimeWarpComponent::UPlayerTimeWarpComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.SetTickFunctionEnable(false);
}

void UPlayerTimeWarpComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<APlayerCharacterBase>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("PlayerTimeWarpComponent: OwnerCharacter is nullptr")
		);
		return;
	}

	DefenseComponent = OwnerCharacter->GetDefenseComponent();
	if (!DefenseComponent)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("PlayerTimeWarpComponent: DefenseComponent is nullptr")
		);
		return;
	}

	// 아래 OnEvadeSuccess는 DefenseComponent 쪽에 추가할 native delegate.
	DefenseComponent->OnEvadeSuccess.AddUObject(
		this,
		&UPlayerTimeWarpComponent::HandleEvadeSuccess
	);
}

void UPlayerTimeWarpComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(
		DeltaTime,
		TickType,
		ThisTickFunction
	);

	if (!bActive)
	{
		SetComponentTickEnabled(false);
		return;
	}

	UpdateVisualEffects();

	const double Now = FPlatformTime::Seconds();

	if (Now >= EndRealTime)
	{
		DeactivateTimeWarp();
	}
}

void UPlayerTimeWarpComponent::HandleEvadeSuccess(
	const FIncomingAttackContext& Context
)
{
	if (!CanActivateTimeWarp())
	{
		return;
	}

	ActivateTimeWarp(Context);
}

bool UPlayerTimeWarpComponent::CanActivateTimeWarp() const
{
	if (!OwnerCharacter || !TimeWarpData)
	{
		return false;
	}

	if (IsBlockedByState())
	{
		return false;
	}

	const double Now = FPlatformTime::Seconds();

	if (!TimeWarpData->bRefreshDurationIfAlreadyActive && bActive)
	{
		return false;
	}

	if (Now < NextAvailableRealTime)
	{
		return false;
	}

	return true;
}

bool UPlayerTimeWarpComponent::IsBlockedByState() const
{
	if (!OwnerCharacter)
	{
		return true;
	}

	UStateTagComponent* StateComponent =
		OwnerCharacter->GetStateTagComponent();

	if (!StateComponent)
	{
		return true;
	}

	if (StateComponent->HasStateTagExact(
		CombatTags::State_Hit_Dead))
	{
		return true;
	}

	if (StateComponent->HasStateTagExact(
		CombatTags::State_Hit_PostureBroken))
	{
		return true;
	}

	if (StateComponent->HasStateTagExact(
		CombatTags::State_Action_Executing))
	{
		return true;
	}

	return false;
}

void UPlayerTimeWarpComponent::ActivateTimeWarp(
	const FIncomingAttackContext& Context
)
{
	if (!OwnerCharacter || !TimeWarpData)
	{
		return;
	}

	const double Now = FPlatformTime::Seconds();
	VisualStartRealTime = Now;

	if (bActive)
	{
		if (TimeWarpData->bRefreshDurationIfAlreadyActive)
		{
			EndRealTime = Now + TimeWarpData->Duration;
		}

		return;
	}

	bActive = true;

	EndRealTime =
		Now + TimeWarpData->Duration;

	NextAvailableRealTime =
		Now + TimeWarpData->Cooldown;

	ApplyTimeScale();
	StartVisualEffects();

	SetComponentTickEnabled(true);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("TimeWarp Activated")
	);
}

void UPlayerTimeWarpComponent::DeactivateTimeWarp()
{
	if (!bActive)
	{
		return;
	}

	bActive = false;

	RestoreTimeScale();
	StopVisualEffects();

	SetComponentTickEnabled(false);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("TimeWarp Deactivated")
	);
}

void UPlayerTimeWarpComponent::ApplyTimeScale()
{
	if (!OwnerCharacter || !TimeWarpData || !GetWorld())
	{
		return;
	}

	// 월드 시간은 건드리지 않는다.
	// 플레이어도 정상 속도 그대로 둔다.
	OwnerCharacter->CustomTimeDilation = 1.0f;

	ApplySelectiveTimeScale();
}

void UPlayerTimeWarpComponent::RestoreTimeScale()
{
	RestoreSelectiveTimeScale();

	if (OwnerCharacter)
	{
		OwnerCharacter->CustomTimeDilation = 1.0f;
	}
}

void UPlayerTimeWarpComponent::StartVisualEffects()
{
	if (!OwnerCharacter || !TimeWarpData)
	{
		return;
	}

	if (TimeWarpData->StartSound)
	{
		UKatanaSoundManagerSubsystem::PlaySoundAtLocation(
			this,
			EAudioType::SFX,
			TimeWarpData->StartSound,
			OwnerCharacter->GetActorLocation()
		);
	}

	if (TimeWarpData->PlayerTrailEffect)
	{
		USkeletalMeshComponent* Mesh =
			OwnerCharacter->GetMesh();

		if (Mesh)
		{
			ActiveTrailComponent =
				UNiagaraFunctionLibrary::SpawnSystemAttached(
					TimeWarpData->PlayerTrailEffect,
					Mesh,
					TimeWarpData->TrailAttachSocketName,
					FVector::ZeroVector,
					FRotator::ZeroRotator,
					EAttachLocation::SnapToTarget,
					true
				);
		}
	}

	if (UPlayerCameraComponent* CameraComponent =
		OwnerCharacter->GetPlayerCameraComponent())
	{
		CameraComponent->BeginTimeWarpPostProcess(
			TimeWarpData->PostProcessMaterial,
			TimeWarpData->PostProcessBlendWeight
		);
	}
}

void UPlayerTimeWarpComponent::StopVisualEffects()
{
	if (!OwnerCharacter || !TimeWarpData)
	{
		return;
	}

	if (TimeWarpData->EndSound)
	{
		UKatanaSoundManagerSubsystem::PlaySoundAtLocation(
			this,
			EAudioType::SFX,
			TimeWarpData->EndSound,
			OwnerCharacter->GetActorLocation()
		);
	}

	if (ActiveTrailComponent)
	{
		ActiveTrailComponent->Deactivate();
		ActiveTrailComponent = nullptr;
	}

	if (UPlayerCameraComponent* CameraComponent =
		OwnerCharacter->GetPlayerCameraComponent())
	{
		CameraComponent->EndTimeWarpPostProcess();
	}
}

void UPlayerTimeWarpComponent::ApplySelectiveTimeScale()
{
	if (!TimeWarpData || !GetWorld())
	{
		return;
	}

	AffectedActorCaches.Empty();

	const float SafeTimeScale =
		FMath::Clamp(
			TimeWarpData->AffectedActorTimeScale,
			0.05f,
			1.0f
		);

	for (const TSubclassOf<AActor>& ActorClass :
		TimeWarpData->AffectedActorClasses)
	{
		if (!ActorClass)
		{
			continue;
		}

		TArray<AActor*> FoundActors;

		UGameplayStatics::GetAllActorsOfClass(
			GetWorld(),
			ActorClass,
			FoundActors
		);

		for (AActor* Actor : FoundActors)
		{
			if (!ShouldAffectActor(Actor))
			{
				continue;
			}

			if (IsActorAlreadyCached(Actor))
			{
				continue;
			}

			FTimeWarpDilationCache Cache;
			Cache.Actor = Actor;
			Cache.PreviousCustomTimeDilation =
				Actor->CustomTimeDilation;

			AffectedActorCaches.Add(Cache);

			Actor->CustomTimeDilation = SafeTimeScale;
		}
	}
}

void UPlayerTimeWarpComponent::RestoreSelectiveTimeScale()
{
	for (const FTimeWarpDilationCache& Cache :
		AffectedActorCaches)
	{
		if (!IsValid(Cache.Actor))
		{
			continue;
		}

		Cache.Actor->CustomTimeDilation =
			Cache.PreviousCustomTimeDilation;
	}

	AffectedActorCaches.Empty();
}

bool UPlayerTimeWarpComponent::ShouldAffectActor(
	AActor* Actor) const
{
	if (!IsValid(Actor))
	{
		return false;
	}

	if (Actor == OwnerCharacter)
	{
		return false;
	}

	return true;
}

bool UPlayerTimeWarpComponent::IsActorAlreadyCached(
	AActor* Actor) const
{
	for (const FTimeWarpDilationCache& Cache :
		AffectedActorCaches)
	{
		if (Cache.Actor == Actor)
		{
			return true;
		}
	}

	return false;
}

void UPlayerTimeWarpComponent::UpdateVisualEffects()
{
	if (!OwnerCharacter)
	{
		return;
	}

	UPlayerCameraComponent* CameraComponent =
		OwnerCharacter->GetPlayerCameraComponent();

	if (!CameraComponent)
	{
		return;
	}

	const double Now =
		FPlatformTime::Seconds();

	const float Elapsed =
		static_cast<float>(Now - VisualStartRealTime);

	const float Alpha =
		FMath::Clamp(
			Elapsed / FMath::Max(SurfaceSweepDuration, 0.001f),
			0.0f,
			1.0f
		);

	const float SweepAlpha =
		FMath::InterpEaseOut(
			0.0f,
			1.0f,
			Alpha,
			1.4f
		);

	const float ProgressDistance =
		FMath::Lerp(
			SweepStartDistance,
			SweepEndDistance,
			SweepAlpha
		);

	const float FadeOutAlpha =
		FMath::Clamp(
			(Alpha - SweepFadeOutStartAlpha) /
			FMath::Max(1.0f - SweepFadeOutStartAlpha, 0.001f),
			0.0f,
			1.0f
		);

	const float EffectAlpha =
		1.0f - FadeOutAlpha;

	FVector SweepDirection =
		OwnerCharacter->GetControlRotation().Vector();

	SweepDirection.Z = 0.0f;

	if (!SweepDirection.Normalize())
	{
		SweepDirection =
			OwnerCharacter->GetActorForwardVector();
	}

	SweepDirection.Z = 0.0f;
	SweepDirection.Normalize();

	CameraComponent->UpdateTimeWarpPostProcess(
		OwnerCharacter->GetActorLocation(),
		SweepDirection,
		ProgressDistance,
		EffectAlpha
	);
}