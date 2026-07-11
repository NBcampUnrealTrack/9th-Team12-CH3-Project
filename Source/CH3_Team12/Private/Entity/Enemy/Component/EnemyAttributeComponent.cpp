// Fill out your copyright notice in the Description page of Project Settings.


#include "Entity/Enemy/Component/EnemyAttributeComponent.h"

#include "Engine/World.h"
#include "TimerManager.h"
#include "Entity/Enemy/EnemyCharacterBase.h"
#include "Entity/Player/StateTagComponent.h"
#include "GameplayTags/CombatGameplayTags.h"

UEnemyAttributeComponent::UEnemyAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEnemyAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<AEnemyCharacterBase>(GetOwner());

	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyAttributeComponent owner is not EnemyCharacterBase."));
		return;
	}

	StateComponent = OwnerCharacter->GetStateTagComponent();

	if (!StateComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("EnemyAttributeComponent could not find StateTagComponent."));
	}
	
	if (AttributeData)
	{
		MaxHealth = AttributeData->MaxHealth;
		MaxPosture = AttributeData->MaxPosture;
		PostureRecoveryRate = AttributeData->PostureRecoveryRate;
		PostureRecoveryDelay = AttributeData->PostureRecoveryDelay;
		PostureBreakDuration = AttributeData->PostureBreakDuration;
	}

	ResetAttributes();
}

void UEnemyAttributeComponent::TickComponent(
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

	UpdatePostureRecovery(DeltaTime);
}

void UEnemyAttributeComponent::ResetAttributes()
{
	bIsDead = false;
	bIsPostureBroken = false;

	CurrentHealth = MaxHealth;
	CurrentPosture = 0.0f;

	if (StateComponent)
	{
		StateComponent->RemoveStateTag(CombatTags::State_Hit_Dead);
		StateComponent->RemoveStateTag(CombatTags::State_Hit_PostureBroken);
	}

	OnEnemyHealthChanged.Broadcast(
		CurrentHealth,
		MaxHealth
	);

	OnEnemyPostureChanged.Broadcast(
		CurrentPosture,
		MaxPosture
	);
}

void UEnemyAttributeComponent::ApplyAttributeDamage(
	float HealthDamage,
	float PostureDamage
)
{
	if (bIsDead)
	{
		return;
	}

	ApplyHealthDamage(HealthDamage);

	if (bIsDead)
	{
		return;
	}

	ApplyPostureDamage(PostureDamage);
}

void UEnemyAttributeComponent::ApplyHealthDamage(float HealthDamage)
{
	if (bIsDead)
	{
		return;
	}

	if (HealthDamage <= 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(
		CurrentHealth - HealthDamage,
		0.0f,
		MaxHealth
	);

	OnEnemyHealthChanged.Broadcast(
		CurrentHealth,
		MaxHealth
	);

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}
}

void UEnemyAttributeComponent::ApplyPostureDamage(float PostureDamage)
{
	if (bIsDead || bIsPostureBroken)
	{
		return;
	}

	if (PostureDamage <= 0.0f)
	{
		return;
	}

	CurrentPosture = FMath::Clamp(
		CurrentPosture + PostureDamage,
		0.0f,
		MaxPosture
	);

	LastPostureDamageTime = GetWorld()
		? GetWorld()->GetTimeSeconds()
		: 0.0f;

	OnEnemyPostureChanged.Broadcast(
		CurrentPosture,
		MaxPosture
	);

	if (CurrentPosture >= MaxPosture)
	{
		BreakPosture();
	}
}

void UEnemyAttributeComponent::Heal(float HealAmount)
{
	if (bIsDead)
	{
		return;
	}

	if (HealAmount <= 0.0f)
	{
		return;
	}

	CurrentHealth = FMath::Clamp(
		CurrentHealth + HealAmount,
		0.0f,
		MaxHealth
	);

	OnEnemyHealthChanged.Broadcast(
		CurrentHealth,
		MaxHealth
	);
}

void UEnemyAttributeComponent::RecoverPosture(float RecoveryAmount)
{
	if (bIsDead || bIsPostureBroken)
	{
		return;
	}

	if (RecoveryAmount <= 0.0f)
	{
		return;
	}

	CurrentPosture = FMath::Clamp(
		CurrentPosture - RecoveryAmount,
		0.0f,
		MaxPosture
	);

	OnEnemyPostureChanged.Broadcast(
		CurrentPosture,
		MaxPosture
	);
}

void UEnemyAttributeComponent::UpdatePostureRecovery(float DeltaTime)
{
	if (bIsDead || bIsPostureBroken)
	{
		return;
	}

	if (CurrentPosture <= 0.0f)
	{
		return;
	}

	if (!GetWorld())
	{
		return;
	}

	const float CurrentTime = GetWorld()->GetTimeSeconds();
	const float TimeSinceLastPostureDamage =
		CurrentTime - LastPostureDamageTime;

	if (TimeSinceLastPostureDamage < PostureRecoveryDelay)
	{
		return;
	}

	const float RecoveryAmount =
		PostureRecoveryRate * DeltaTime;

	RecoverPosture(RecoveryAmount);
}

void UEnemyAttributeComponent::BreakPosture()
{
	if (bIsDead || bIsPostureBroken)
	{
		return;
	}

	bIsPostureBroken = true;
	CurrentPosture = MaxPosture;

	if (StateComponent)
	{
		StateComponent->AddStateTag(
			CombatTags::State_Hit_PostureBroken
		);
	}

	OnEnemyPostureChanged.Broadcast(
		CurrentPosture,
		MaxPosture
	);

	OnEnemyPostureBroken.Broadcast();
	
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			PostureBreakTimerHandle,
			this,
			&UEnemyAttributeComponent::RecoverFromPostureBreak,
			PostureBreakDuration,
			false
		);
	}
}

void UEnemyAttributeComponent::RecoverFromPostureBreak()
{
	if (bIsDead)
	{
		return;
	}

	bIsPostureBroken = false;
	CurrentPosture = 0.0f;

	if (StateComponent)
	{
		StateComponent->RemoveStateTag(
			CombatTags::State_Hit_PostureBroken
		);
	}

	OnEnemyPostureChanged.Broadcast(
		CurrentPosture,
		MaxPosture
	);

	OnEnemyPostureRecovered.Broadcast();
}

void UEnemyAttributeComponent::Die()
{
	if (bIsDead)
	{
		return;
	}

	bIsDead = true;
	bIsPostureBroken = false;

	CurrentHealth = 0.0f;
	CurrentPosture = 0.0f;

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(
			PostureBreakTimerHandle
		);
	}

	if (StateComponent)
	{
		StateComponent->AddStateTag(
			CombatTags::State_Hit_Dead
		);

		StateComponent->RemoveStateTag(
			CombatTags::State_Hit_PostureBroken
		);
	}

	OnEnemyHealthChanged.Broadcast(
		CurrentHealth,
		MaxHealth
	);

	OnEnemyPostureChanged.Broadcast(
		CurrentPosture,
		MaxPosture
	);

	OnEnemyDeath.Broadcast();
}
