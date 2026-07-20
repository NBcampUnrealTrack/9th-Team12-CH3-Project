// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/GameMode/KatanaGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TimerManager.h"

void AKatanaGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void AKatanaGameMode::ChangeGamePlayRate(UWorld* InWorld, float InGamePlayRate)
{
	if (InWorld == nullptr)
	{
		return;
	}
	
	UGameplayStatics::SetGlobalTimeDilation(InWorld, InGamePlayRate);
}

void AKatanaGameMode::SlowMotion(UWorld* InWorld, float InGamePlayRate, float InTime)
{
	if (InWorld == nullptr)
	{
		return;
	}
	
	ChangeGamePlayRate(InWorld, InGamePlayRate);
	
	FTimerHandle TimerHandle;
	InWorld->GetTimerManager().SetTimer(
		TimerHandle,
		[this, InWorld]()
		{
			ChangeGamePlayRate(InWorld, 1.0f);
		},
		InTime,
		false);
	
}
