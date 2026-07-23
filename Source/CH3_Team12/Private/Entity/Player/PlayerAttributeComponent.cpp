#include "Entity/Player/PlayerAttributeComponent.h"

#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/StateTagComponent.h"
#include "GameplayTags/CombatGameplayTags.h"

UPlayerAttributeComponent::UPlayerAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<APlayerCharacterBase>(GetOwner());

	if (!OwnerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerAttributeComponent owner is not PlayerCharacterBase."));
		return;
	}

	StateComponent = OwnerCharacter->GetStateTagComponent();

	if (!StateComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerAttributeComponent could not find StateTagComponent."));
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

void UPlayerAttributeComponent::TickComponent(
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

void UPlayerAttributeComponent::ResetAttributes()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(
			PostureBreakTimerHandle
		);
	}

	bIsDead = false;
	bIsPostureBroken = false;

	CurrentHealth = MaxHealth;
	CurrentPosture = 0.0f;

	if (StateComponent)
	{
		StateComponent->RemoveStateTag(CombatTags::State_Hit_Dead);
		StateComponent->RemoveStateTag(CombatTags::State_Hit_PostureBroken);
	}

	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
	OnPostureChanged.Broadcast(CurrentPosture, MaxPosture);
}

void UPlayerAttributeComponent::ApplyAttributeDamage(
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

void UPlayerAttributeComponent::ApplyHealthDamage(float HealthDamage)
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

	OnHealthChanged.Broadcast(
		CurrentHealth,
		MaxHealth
	);

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}
}

void UPlayerAttributeComponent::ApplyPostureDamage(float PostureDamage)
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

	OnPostureChanged.Broadcast(
		CurrentPosture,
		MaxPosture
	);

	if (CurrentPosture >= MaxPosture)
	{
		BreakPosture();
	}
}

void UPlayerAttributeComponent::Heal(float HealAmount)
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

	OnHealthChanged.Broadcast(
		CurrentHealth,
		MaxHealth
	);
}

void UPlayerAttributeComponent::RecoverPosture(float RecoveryAmount)
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

	OnPostureChanged.Broadcast(
		CurrentPosture,
		MaxPosture
	);
}

void UPlayerAttributeComponent::UpdatePostureRecovery(float DeltaTime)
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

void UPlayerAttributeComponent::BreakPosture()
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

	OnPostureChanged.Broadcast(
		CurrentPosture,
		MaxPosture
	);

	OnPostureBroken.Broadcast();

	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			PostureBreakTimerHandle,
			this,
			&UPlayerAttributeComponent::RecoverFromPostureBreak,
			PostureBreakDuration,
			false
		);
	}
}

void UPlayerAttributeComponent::RecoverFromPostureBreak()
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

	OnPostureChanged.Broadcast(
		CurrentPosture,
		MaxPosture
	);

	OnPostureRecovered.Broadcast();
}

void UPlayerAttributeComponent::Die()
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

	OnHealthChanged.Broadcast(
		CurrentHealth,
		MaxHealth
	);

	OnPostureChanged.Broadcast(
		CurrentPosture,
		MaxPosture
	);

	OnDead.Broadcast();
}
