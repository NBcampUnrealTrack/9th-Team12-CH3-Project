#include "Entity/Enemy/DamageDummy.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/PlayerAttributeComponent.h"
#include "Entity/Player/PlayerAttackComponent.h"
#include "Entity/Player/PlayerDefenseComponent.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"

ADamageDummy::ADamageDummy()
{
	PrimaryActorTick.bCanEverTick = false;

	AutoPossessAI = EAutoPossessAI::Disabled;
	AIControllerClass = nullptr;

	DummyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DummyMesh"));
	DummyMesh->SetupAttachment(GetRootComponent());
	
	AttackSphere = CreateDefaultSubobject<USphereComponent>("AttackSphere");
	AttackSphere->SetupAttachment(GetRootComponent());
	
	AttackSphere->SetSphereRadius(300.f);

	AttackSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	AttackSphere->SetCollisionResponseToAllChannels(ECR_Ignore);

	AttackSphere->SetCollisionResponseToChannel(
		ECC_Pawn,
		ECR_Overlap);
}

void ADamageDummy::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&ADamageDummy::PrepareAttack,
		AttackInterval,
		true);
}

void ADamageDummy::PrepareAttack()
{
	if (WarningMaterial)
	{
		DummyMesh->SetMaterial(0, WarningMaterial);
	}

	DrawDebugSphere(
		GetWorld(),
		AttackSphere->GetComponentLocation(),
		AttackSphere->GetScaledSphereRadius(),
		32,
		FColor::Yellow,
		false,
		WarningTime,
		0,
		2.f);

	GetWorldTimerManager().SetTimer(
		WarningTimerHandle,
		this,
		&ADamageDummy::Attack,
		WarningTime,
		false);
}

void ADamageDummy::Attack()
{
	if (NormalMaterial)
	{
		DummyMesh->SetMaterial(0, NormalMaterial);
	}

	DrawDebugSphere(
		GetWorld(),
		AttackSphere->GetComponentLocation(),
		AttackSphere->GetScaledSphereRadius(),
		32,
		FColor::Red,
		false,
		0.2f);

	TArray<AActor*> Actors;
	AttackSphere->GetOverlappingActors(
		Actors,
		APlayerCharacterBase::StaticClass());

	for (AActor* Actor : Actors)
	{
		APlayerCharacterBase* Player =
			Cast<APlayerCharacterBase>(Actor);

		if (!Player)
		{
			continue;
		}

		UPlayerAttackComponent* AttackComponent =
			Player->GetCombatComponent();

		if (!AttackComponent)
		{
			continue;
		}

		//------------------------------------
		// 공격 정보 생성
		//------------------------------------

		FIncomingAttackContext Context;

		Context.Attacker = this;

		Context.AttackInfo.Damage = HealthDamage;
		Context.AttackInfo.PostureDamage = PostureDamage;

		Context.AttackInfo.bCanBeGuarded = true;
		Context.AttackInfo.bCanBeParried = true;

		//------------------------------------
		// 공격 방향
		//------------------------------------

		FVector AttackDirection =
			Player->GetActorLocation() - GetActorLocation();

		AttackDirection.Z = 0.f;
		AttackDirection.Normalize();

		Context.AttackWorldDirection = AttackDirection;

		//------------------------------------
		// Hit 위치
		//------------------------------------

		Context.Hit.ImpactPoint =
			Player->GetActorLocation();

		Context.Hit.ImpactNormal =
			-AttackDirection;

		//------------------------------------
		UPlayerDefenseComponent* DefenseComponent =
			Player->GetDefenseComponent();

		if (!DefenseComponent)
		{
			continue;
		}
		DefenseComponent->ResolveIncomingAttack(Context);
	}
}

