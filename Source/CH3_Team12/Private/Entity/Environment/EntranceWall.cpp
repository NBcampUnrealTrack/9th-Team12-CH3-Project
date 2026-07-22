#include "Entity/Environment/EntranceWall.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Entity/Player/PlayerCharacterBase.h"

AEntranceWall::AEntranceWall()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	FogMesh = CreateDefaultSubobject<UStaticMeshComponent>("FogMesh");
	FogMesh->SetupAttachment(Root);

	EnterTrigger = CreateDefaultSubobject<UBoxComponent>("EnterTrigger");
	EnterTrigger->SetupAttachment(Root);

	ExitTrigger = CreateDefaultSubobject<UBoxComponent>("ExitTrigger");
	ExitTrigger->SetupAttachment(Root);

	FogMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	EnterTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ExitTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	EnterTrigger->SetGenerateOverlapEvents(true);
	ExitTrigger->SetGenerateOverlapEvents(true);
}

void AEntranceWall::BeginPlay()
{
	Super::BeginPlay();
	
	EnterTrigger->OnComponentBeginOverlap.AddDynamic(
		this,
		&AEntranceWall::OnEnterTriggerBegin);

	ExitTrigger->OnComponentBeginOverlap.AddDynamic(
		this,
		&AEntranceWall::OnExitTriggerBegin);

	FogMid = FogMesh->CreateDynamicMaterialInstance(0);
	
	if (FogMid)
	{
		FogMid->SetScalarParameterValue(
			OpacityParameter,
			0.0f);
	}
	
	FogMesh->SetCollisionProfileName(TEXT("BlockAll"));
	FogMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEntranceWall::OnEnterTriggerBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bActivated)
		return;

	APlayerCharacterBase* Player = Cast<APlayerCharacterBase>(OtherActor);
	if (!Player)
	{
		return;
	}
	
	bPlayerEntered = true;
}

void AEntranceWall::OnExitTriggerBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!bPlayerEntered)
		return;

	if (bActivated)
		return;

	APlayerCharacterBase* Player = Cast<APlayerCharacterBase>(OtherActor);
	if (!Player)
	{
		return;
	}
	
	ActivateFogWall();
}

void AEntranceWall::ActivateFogWall()
{
	bActivated = true;

	if (FogMid)
	{
		FogMid->SetScalarParameterValue(
			OpacityParameter,
			ActivatedOpacity);
	}

	FogMesh->SetCollisionEnabled(
		ECollisionEnabled::QueryAndPhysics);

	FogMesh->SetCollisionResponseToChannel(
		ECC_Pawn,
		ECR_Block);

	EnterTrigger->SetCollisionEnabled(
		ECollisionEnabled::NoCollision);

	ExitTrigger->SetCollisionEnabled(
		ECollisionEnabled::NoCollision);
	
	
}