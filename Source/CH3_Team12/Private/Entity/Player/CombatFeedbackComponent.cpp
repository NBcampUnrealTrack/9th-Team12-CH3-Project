#include "Entity/Player/CombatFeedbackComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerEquipmentComponent.h"
#include "Entity/Weapon/WeaponBase.h"

#include "Components/StaticMeshComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Framework/Subsystem/KatanaSoundManagerSubsystem.h"

UCombatFeedbackComponent::UCombatFeedbackComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatFeedbackComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<APlayerCharacterBase>(GetOwner());

	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("CombatFeedbackComponent : OwnerCharacter is nullptr"));
		return;
	}

	EquipmentComponent = OwnerCharacter->GetEquipmentComponent();

	if (!EquipmentComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("CombatFeedbackComponent : EquipmentComponent is nullptr"));
	}
}

void UCombatFeedbackComponent::EndPlay(
	const EEndPlayReason::Type EndPlayReason)
{
	ResetHitStop();

	Super::EndPlay(EndPlayReason);
}

void UCombatFeedbackComponent::PlayCombatFeedback(
	const FIncomingAttackContext& Context,
	const UPlayerCombatFeedbackDataAsset* FeedbackAsset,
	const FCombatFeedbackData& Feedback)
{
	if (!GetWorld() || !FeedbackAsset)
	{
		return;
	}

	const FVector Location =
		MakeCombatEffectLocation(
			Context,
			FeedbackAsset,
			Feedback.LocationMode
		);

	const FRotator Rotation =
		MakeCombatEffectRotation(
			Context,
			Feedback.RotationMode
		);

	if (Feedback.Effect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			Feedback.Effect,
			Location,
			Rotation
		);
	}

	if (Feedback.Sound)
	{
		UKatanaSoundManagerSubsystem::PlaySoundAtLocation(
			this,
			EAudioType::SFX,
			Feedback.Sound,
			Location
		);
	}

	ApplyHitStopFromFeedback(Context, Feedback);
}

void UCombatFeedbackComponent::ApplyHitStopFromFeedback(
	const FIncomingAttackContext& Context,
	const FCombatFeedbackData& Feedback)
{
	if (Feedback.HitStopDuration <= 0.0f)
	{
		return;
	}

	if (Feedback.HitStopTargetPolicy ==
		ECombatHitStopTargetPolicy::None)
	{
		return;
	}

	FCombatHitStopSpec Spec;
	Spec.Attacker = Context.Attacker;
	Spec.Defender = OwnerCharacter;
	Spec.Duration = Feedback.HitStopDuration;
	Spec.AttackerTimeDilation = Feedback.AttackerTimeDilation;
	Spec.DefenderTimeDilation = Feedback.DefenderTimeDilation;

	switch (Feedback.HitStopTargetPolicy)
	{
	case ECombatHitStopTargetPolicy::AttackerOnly:
		Spec.bAffectAttacker = true;
		break;

	case ECombatHitStopTargetPolicy::DefenderOnly:
		Spec.bAffectDefender = true;
		break;

	case ECombatHitStopTargetPolicy::Both:
		Spec.bAffectAttacker = true;
		Spec.bAffectDefender = true;
		break;

	case ECombatHitStopTargetPolicy::None:
	default:
		break;
	}

	ApplyHitStop(Spec);
}

void UCombatFeedbackComponent::ApplyHitStop(
	const FCombatHitStopSpec& Spec)
{
	if (!GetWorld())
	{
		return;
	}

	if (Spec.Duration <= 0.0f)
	{
		return;
	}

	ResetHitStop();

	HitStopActors.Reset();

	if (Spec.bAffectAttacker)
	{
		ApplyTimeDilationToActor(
			Spec.Attacker,
			Spec.AttackerTimeDilation
		);
	}

	if (Spec.bAffectDefender)
	{
		ApplyTimeDilationToActor(
			Spec.Defender,
			Spec.DefenderTimeDilation
		);
	}

	GetWorld()->GetTimerManager().SetTimer(
		HitStopTimerHandle,
		this,
		&UCombatFeedbackComponent::ResetHitStop,
		Spec.Duration,
		false
	);
}

void UCombatFeedbackComponent::ApplyTimeDilationToActor(
	AActor* Actor,
	float TimeDilation)
{
	if (!IsValid(Actor))
	{
		return;
	}

	TimeDilation = FMath::Clamp(
		TimeDilation,
		0.01f,
		1.0f
	);

	if (!HitStopActors.Contains(Actor))
	{
		HitStopActors.Add(Actor);
	}

	Actor->CustomTimeDilation = TimeDilation;
}

void UCombatFeedbackComponent::ResetHitStop()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(
			HitStopTimerHandle
		);
	}

	for (TWeakObjectPtr<AActor> ActorPtr : HitStopActors)
	{
		if (AActor* Actor = ActorPtr.Get())
		{
			Actor->CustomTimeDilation = 1.0f;
		}
	}

	HitStopActors.Reset();
}

FVector UCombatFeedbackComponent::MakeCombatEffectLocation(
	const FIncomingAttackContext& Context,
	const UPlayerCombatFeedbackDataAsset* FeedbackAsset,
	ECombatEffectLocationMode LocationMode) const
{
	switch (LocationMode)
	{
	case ECombatEffectLocationMode::DefenderWeaponClashSocket:
		return GetWeaponClashEffectLocation(
			Context,
			FeedbackAsset
		);

	case ECombatEffectLocationMode::DefenderWeaponBladeMiddle:
		if (EquipmentComponent &&
			EquipmentComponent->GetEquippedWeapon())
		{
			const FVector BladeStart =
				EquipmentComponent
				->GetEquippedWeapon()
				->GetBladeStartLocation();

			const FVector BladeEnd =
				EquipmentComponent
				->GetEquippedWeapon()
				->GetBladeEndLocation();

			if (!BladeStart.IsNearlyZero() &&
				!BladeEnd.IsNearlyZero())
			{
				return (BladeStart + BladeEnd) * 0.5f;
			}
		}

		return GetWeaponClashEffectLocation(
			Context,
			FeedbackAsset
		);

	case ECombatEffectLocationMode::DefenderActorCenter:
		return GetFallbackEffectLocation(FeedbackAsset);

	case ECombatEffectLocationMode::HitImpactPoint:
	default:
		return GetHitImpactEffectLocation(
			Context,
			FeedbackAsset
		);
	}
}

FVector UCombatFeedbackComponent::GetWeaponClashEffectLocation(
	const FIncomingAttackContext& Context,
	const UPlayerCombatFeedbackDataAsset* FeedbackAsset) const
{
	if (EquipmentComponent &&
		EquipmentComponent->GetEquippedWeapon())
	{
		if (UStaticMeshComponent* WeaponMesh =
			EquipmentComponent
			->GetEquippedWeapon()
			->GetWeaponMesh())
		{
			if (FeedbackAsset &&
				WeaponMesh->DoesSocketExist(
					FeedbackAsset->WeaponClashEffectSocketName))
			{
				return WeaponMesh->GetSocketLocation(
					FeedbackAsset->WeaponClashEffectSocketName
				);
			}
		}

		const FVector BladeStart =
			EquipmentComponent
			->GetEquippedWeapon()
			->GetBladeStartLocation();

		const FVector BladeEnd =
			EquipmentComponent
			->GetEquippedWeapon()
			->GetBladeEndLocation();

		if (!BladeStart.IsNearlyZero() &&
			!BladeEnd.IsNearlyZero())
		{
			return (BladeStart + BladeEnd) * 0.5f;
		}
	}

	return GetHitImpactEffectLocation(
		Context,
		FeedbackAsset
	);
}

FVector UCombatFeedbackComponent::GetHitImpactEffectLocation(
	const FIncomingAttackContext& Context,
	const UPlayerCombatFeedbackDataAsset* FeedbackAsset) const
{
	if (!Context.Hit.ImpactPoint.IsNearlyZero())
	{
		if (!Context.Hit.ImpactNormal.IsNearlyZero())
		{
			const float Offset =
				FeedbackAsset
					? FeedbackAsset->HitEffectSurfaceOffset
					: 0.0f;

			return Context.Hit.ImpactPoint
				+ Context.Hit.ImpactNormal.GetSafeNormal()
				* Offset;
		}

		return Context.Hit.ImpactPoint;
	}

	return GetFallbackEffectLocation(FeedbackAsset);
}

FVector UCombatFeedbackComponent::GetFallbackEffectLocation(
	const UPlayerCombatFeedbackDataAsset* FeedbackAsset) const
{
	if (!OwnerCharacter)
	{
		return FVector::ZeroVector;
	}

	const float HeightOffset =
		FeedbackAsset
			? FeedbackAsset->FallbackEffectHeightOffset
			: 20.0f;

	return OwnerCharacter->GetActorLocation()
		+ FVector(0.0f, 0.0f, HeightOffset);
}

FRotator UCombatFeedbackComponent::MakeCombatEffectRotation(
	const FIncomingAttackContext& Context,
	ECombatEffectRotationMode RotationMode) const
{
	if (!OwnerCharacter)
	{
		return FRotator::ZeroRotator;
	}

	FVector Direction = OwnerCharacter->GetActorForwardVector();

	switch (RotationMode)
	{
	case ECombatEffectRotationMode::ImpactNormal:
		if (!Context.Hit.ImpactNormal.IsNearlyZero())
		{
			Direction = Context.Hit.ImpactNormal;
		}
		break;

	case ECombatEffectRotationMode::AttackDirection:
		if (!Context.AttackWorldDirection.IsNearlyZero())
		{
			Direction = Context.AttackWorldDirection;
		}
		break;

	case ECombatEffectRotationMode::OppositeAttackDirection:
		if (!Context.AttackWorldDirection.IsNearlyZero())
		{
			Direction = -Context.AttackWorldDirection;
		}
		break;

	case ECombatEffectRotationMode::AttackerToDefender:
		if (Context.Attacker)
		{
			Direction =
				OwnerCharacter->GetActorLocation()
				- Context.Attacker->GetActorLocation();
		}
		break;

	case ECombatEffectRotationMode::DefenderToAttacker:
		if (Context.Attacker)
		{
			Direction =
				Context.Attacker->GetActorLocation()
				- OwnerCharacter->GetActorLocation();
		}
		break;

	case ECombatEffectRotationMode::DefenderForward:
		Direction = OwnerCharacter->GetActorForwardVector();
		break;

	case ECombatEffectRotationMode::WorldUp:
		Direction = FVector::UpVector;
		break;

	case ECombatEffectRotationMode::None:
	default:
		return FRotator::ZeroRotator;
	}

	if (Direction.IsNearlyZero())
	{
		return FRotator::ZeroRotator;
	}

	return FRotationMatrix::MakeFromX(
		Direction.GetSafeNormal()
	).Rotator();
}