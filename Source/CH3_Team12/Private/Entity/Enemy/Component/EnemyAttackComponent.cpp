// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/Component/EnemyAttackComponent.h"

#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "GameFramework/Character.h"
#include "Framework/DataAsset/EnemyAttackDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "Entity/Player/PlayerCombatComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Combat/CombatTypes.h"

// Sets default values for this component's properties
UEnemyAttackComponent::UEnemyAttackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	CurrentPlayingMontage = EEnemyAttackPattern::End;
}


// Called when the game starts
void UEnemyAttackComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UEnemyAttackComponent::ExecuteAttack(AActor* TargetActor, int32 SelectedAction, int32 SelectedPattern)
{
	UE_LOG(LogTemp, Warning, TEXT("ExecuteAttack: Action=%d Pattern=%d"), SelectedAction, SelectedPattern);
	AActor* OwnerActor = GetOwner();

	if (!OwnerActor || !TargetActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteAttack Failed: OwnerActor or TargetActor is null"));
		return false;
	}
	const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteAttack Failed: OwnerCharacter is null"));
		return false;
	}

	if (!bCanAttack)
	{
		return false;
	}

	USkeletalMeshComponent* SkeletalMeshComponent = OwnerCharacter->GetMesh();
	if (SkeletalMeshComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteAttack Failed: SkeletalMesh is invalid."));
		return false;
	}
	
	UAnimInstance* AnimInstance = SkeletalMeshComponent->GetAnimInstance();
	if (!AnimInstance)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteAttack Failed: Animinstance"));
		return false;
	}
	
	if (IsValid(AttackData) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Katana_UEnemyAttackComponent : AttackData is invalid."));
		return false;
	}
	
	const FAttackAnimationData* AttackAnimationData = GetSelectedPatternData(SelectedAction, SelectedPattern);
	if (AttackAnimationData == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Katana_UEnemyAttackComponent : AttackData is invalid."));
		return false;
	}
	
	// if (SelectedAction == 1)
	// {
	// 	// TODO: 일반공격
	// 	// NOTE: 공격Montage 끝나면 OnAttackFinished.Broadcast() 호출
	// 	UE_LOG(LogTemp, Log, TEXT("Execute Normal Attack Pattern: %d"), SelectedPattern);
	// }
	// else if (SelectedAction == 2)
	// {
	// 	// TODO: 강공격
	// 	// NOTE: 공격Montage 끝나면 OnAttackFinished.Broadcast() 호출
	// 	UE_LOG(LogTemp, Log, TEXT("Execute Strong Attack Pattern: %d"), SelectedPattern);
	// }
	
	UAnimMontage* SelectedMontage = AttackAnimationData->Montage;

	float AttackRange = AttackAnimationData->AttackRange;
	float AttackCooldown = AttackAnimationData->AttackCooldown;

	const float DistanceToTarget = FVector::Dist(
		OwnerActor->GetActorLocation(),
		TargetActor->GetActorLocation()
	);

	if (DistanceToTarget > AttackRange)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteAttack Failed: RangeShort"));
		return false;
	}
	if (!SelectedMontage)
	{
		return false;
	}


	GetWorld()->GetTimerManager().SetTimer(
		AttackCooldownTimerHandle,
		this,
		&UEnemyAttackComponent::ResetAttackCooldown,
		AttackCooldown,
		false
	);
	
	const float MontageLength = AnimInstance->Montage_Play(SelectedMontage);
	
	// AnimInstance->Montage_SetNextSection(
	//
	// 	TEXT("Attack1"),
	// 	TEXT("Attack2"),
	// 	SelectedMontage
	// );
	// AnimInstance->Montage_SetNextSection(
	// 	TEXT("Attack2"),
	// 	TEXT("Attack3"),
	// 	SelectedMontage
	// );
	UE_LOG(LogTemp, Warning, TEXT(" MontageLength: %f"), MontageLength);
	if (MontageLength <= 0.f)
	{
		return false;
	}
	
	bCanAttack = false;
	return true;
}

void UEnemyAttackComponent::ResetAttackCooldown()
{
	bCanAttack = true;
}

void UEnemyAttackComponent::OnAttackAnimationEnd()
{
	// UE_DECLARE_GAMEPLAY_TAG_EXTERN(State_Combat_Attacking);
	// AActor* Owner = GetOwner();
	// if (Owner)
}

void UEnemyAttackComponent::FinishAttack()
{
	bCanAttack = true;
	OnAttackFinished.Broadcast();
	
	CurrentPlayingMontage = EEnemyAttackPattern::End;
}

void UEnemyAttackComponent::StartHitCheck()
{
	if (bUseDebugColliderDraw)
	{
		const FAttackAnimationData* AttackAnimationData = GetCurrentPatternData();
	
		TArray<FHitBoxData> HitBoxDatas = AttackAnimationData->HitBoxes;
	
		for (FHitBoxData HitBoxData : HitBoxDatas)
		{
			USkeletalMeshComponent* SkeletalMeshComponent = GetOwnerSkeletalMeshComponent();
			if (SkeletalMeshComponent == nullptr)
				return;
		
			FName SocketName = HitBoxData.ActiveHitSocket;
			FVector SocketLocation{};
			FRotator SocketRotation{};
			SkeletalMeshComponent->GetSocketWorldLocationAndRotation(SocketName, SocketLocation, SocketRotation);
			DrawDebugSphere(GetWorld(), SocketLocation, 150.0f, 16, FColor::White, false, 5.0f);
		}
	}
	
	HitActors.Reset();
	PreviousHitBoxCenters.Reset();
}

void UEnemyAttackComponent::AttackTrace()
{
	AActor* Owner = GetOwner();
	if (Owner == nullptr)
	{
		return;
	}
	
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FAttackAnimationData* AttackAnimationData = GetCurrentPatternData();
	if (AttackAnimationData == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Katana_EnemyAttackComponent_AttackTrace : AttackAnimationData is invalid."));
		return;
	}
	
	TArray<FHitBoxData> HitBoxDatas = AttackAnimationData->HitBoxes;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);
	
	for (FHitBoxData HitBoxData : HitBoxDatas)
	{
		USkeletalMeshComponent* SkeletalMeshComponent = GetOwnerSkeletalMeshComponent();
		if (SkeletalMeshComponent ==nullptr)
			return;
		
		const FCollisionShape CollisionShape = FCollisionShape::MakeSphere(HitBoxData.TraceRadius);
		const int32 SafeSampleCount = FMath::Max(HitBoxData.TraceSampleCount, 2);
		
		FName SocketName = HitBoxData.ActiveHitSocket;
		FVector CurrentHitBoxCenter{};
		FRotator CurrentHitBoxRotation{};
		SkeletalMeshComponent->GetSocketWorldLocationAndRotation(SocketName, CurrentHitBoxCenter, CurrentHitBoxRotation);
		
		for (int32 Index = 0; Index < SafeSampleCount; ++Index)
		{
			const FVector PreviousPoint = PreviousHitBoxCenters.Contains(SocketName) ? 
				PreviousHitBoxCenters[SocketName] : CurrentHitBoxCenter;
			const FVector CurrentPoint = CurrentHitBoxCenter;

			TArray<FHitResult> HitResults;
			const bool bHit = World->SweepMultiByChannel(
				HitResults,
				PreviousPoint,
				CurrentPoint,
				FQuat::Identity,
				TraceChannel,
				CollisionShape,
				Params
			);
			
			if (!bHit)
			{
				if (bUseDebugColliderDraw)
				{
					DrawDebugSphere(GetWorld(), CurrentHitBoxCenter, 150.0f, 16, FColor::Blue, false, 5.0f);
				}
				continue;
			}
			if (bUseDebugColliderDraw)
			{
				DrawDebugSphere(GetWorld(), CurrentHitBoxCenter, 150.0f, 16, FColor::Red, false, 5.0f);
			}
			
			for (const FHitResult& Hit : HitResults)
			{
				AActor* HitActor = Hit.GetActor();

				if (!HitActor || HitActor == Owner)
				{
					continue;
				}

				if (HitActors.Contains(HitActor))
				{
					continue;
				}

				HitActors.Add(HitActor);
				
				FAttackInfo AttackInfo = AttackAnimationData->AttackInfo;
				ProcessHit(Hit, AttackInfo);
			}
		}

		PreviousHitBoxCenters.FindOrAdd(SocketName) = CurrentHitBoxCenter;
	}
}

void UEnemyAttackComponent::EndHitCheck()
{
	if (bUseDebugColliderDraw)
	{
		const FAttackAnimationData* AttackAnimationData = AttackData->GetAttackAnimationData(0);
	
		TArray<FHitBoxData> HitBoxDatas = AttackAnimationData->HitBoxes;
	
		for (FHitBoxData HitBoxData : HitBoxDatas)
		{
			USkeletalMeshComponent* SkeletalMeshComponent = GetOwnerSkeletalMeshComponent();
			if (SkeletalMeshComponent == nullptr)
				return;
		
			FName SocketName = HitBoxData.ActiveHitSocket;
			FVector SocketLocation{};
			FRotator SocketRotation{};
			SkeletalMeshComponent->GetSocketWorldLocationAndRotation(SocketName, SocketLocation, SocketRotation);
			DrawDebugSphere(GetWorld(), SocketLocation, 150.0f, 16, FColor::White, false, 5.0f);
		}
	}
	
	HitActors.Reset();
	PreviousHitBoxCenters.Reset();
}

// Player에게 데미지 주기 위해 수정 필요
void UEnemyAttackComponent::ProcessHit(const FHitResult& InHitResult, const FAttackInfo& InAttackInfo)
{
	AActor* HitActor = InHitResult.GetActor();
	if (!HitActor)
	{
		return;
	}
	
	APawn* Owner = Cast<APawn>(GetOwner());
	if (Owner == nullptr)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Katana_EnemyAttackComponent_ProcessHit, Hit : %s"), *HitActor->GetName());

	if (APlayerCharacterBase* PlayerCharacter = Cast<APlayerCharacterBase>(HitActor))
	{
		if (UPlayerCombatComponent* PlayerCombatComponent = PlayerCharacter->FindComponentByClass<UPlayerCombatComponent>())
		{
			FIncomingAttackContext IncomingAttackContext;
			IncomingAttackContext.Attacker = GetOwner();
			IncomingAttackContext.Hit = InHitResult;
			IncomingAttackContext.AttackInfo = InAttackInfo;
			
			PlayerCombatComponent->ResolveIncomingAttack(IncomingAttackContext);	
		}
	}
	else
	{
		UGameplayStatics::ApplyDamage(
			HitActor,
			25.0f,
			Owner->GetController(),
			Owner,
			nullptr
		);
	}
}

void UEnemyAttackComponent::StopAttackMontage()
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}

	UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
	if (!AnimInstance)
	{
		return;
	}

	AnimInstance->Montage_Stop(0.15f);
	FinishAttack();
}

const FAttackAnimationData* UEnemyAttackComponent::GetCurrentPatternData()
{
	if (AttackData == nullptr
		|| CurrentPlayingMontage == EEnemyAttackPattern::End)
		return nullptr;
	
	return AttackData->GetAttackAnimationData(static_cast<uint8>(CurrentPlayingMontage));
}

const FAttackAnimationData* UEnemyAttackComponent::GetSelectedPatternData(int32 InSelectedAction, int32 InSelectedPattern)
{
	if (AttackData == nullptr)
		return nullptr;
	
	if (InSelectedAction == 1 && InSelectedPattern == 0)
	{
		// NormalAttackMontage0 
		//SelectedMontage = NormalAttack0;
		CurrentPlayingMontage = EEnemyAttackPattern::NormalAttack_1;
	}
	else if (InSelectedAction == 1 && InSelectedPattern == 1)
	{
		// NormalAttackMontage1 
		//SelectedMontage = NormalAttack0;
		CurrentPlayingMontage = EEnemyAttackPattern::NormalAttack_2;
	}
	else if (InSelectedAction == 2 && InSelectedPattern == 0)
	{
		// StrongAttackMontage0 
		//SelectedMontage = NormalAttack0;
		CurrentPlayingMontage = EEnemyAttackPattern::NormalAttack_1;
	}
	else if (InSelectedAction == 2 && InSelectedPattern == 1)
	{
		// FarStrongAttackMontage 재생
		//SelectedMontage = FarStrongAttack;
		CurrentPlayingMontage = EEnemyAttackPattern::FarStrongAttack;
	}
	
	return AttackData->GetAttackAnimationData(static_cast<int8>(CurrentPlayingMontage));
}

USkeletalMeshComponent* UEnemyAttackComponent::GetOwnerSkeletalMeshComponent()
{
	const ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteAttack Failed: OwnerCharacter is null"));
		return nullptr;
	}
		
	USkeletalMeshComponent* SkeletalMeshComponent = OwnerCharacter->GetMesh();
	if (SkeletalMeshComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("ExecuteAttack Failed: SkeletalMesh is invalid."));
		return nullptr;
	}
	
	return SkeletalMeshComponent;
}

void UEnemyAttackComponent::ResetComboCount()
{
	ComboCount = 0; 
}
