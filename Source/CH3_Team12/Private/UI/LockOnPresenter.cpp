#include "UI/LockOnPresenter.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UI/LockOnWidget.h"


void ULockOnPresenter::Initialize(AActor* InTargetActor, ULockOnWidget* InWidget)
{
	LockOnWidget = InWidget;
	TargetActor = InTargetActor;
	TargetOffset = FVector(0.0f, 0.0f, 0.0f); // 보정값

	if (InWidget)
	{
		CachedPlayerController = UGameplayStatics::GetPlayerController(InWidget->GetWorld(), 0);
		InWidget->AddToViewport();
		bCanTick = true;
	}
}

void ULockOnPresenter::Dispose()
{
	bCanTick = false;

	if (LockOnWidget.IsValid())
	{
		LockOnWidget->RemoveFromParent();
	}

	LockOnWidget = nullptr;
	TargetActor = nullptr;
	CachedPlayerController = nullptr;
}

void ULockOnPresenter::Tick(float DeltaTime)
{
	if (!LockOnWidget.IsValid() || !TargetActor.IsValid() || !CachedPlayerController.IsValid())
		return;

	const FVector WorldPosition = TargetActor->GetActorLocation() + TargetOffset;
	FVector2D ScreenPosition;

	bool bIsOnScreen = CachedPlayerController->ProjectWorldLocationToScreen(WorldPosition, ScreenPosition);

	if (bIsOnScreen)
	{
		int32 SizeX, SizeY;
		CachedPlayerController->GetViewportSize(SizeX, SizeY);

		if (ScreenPosition.X < 0 || ScreenPosition.Y < 0 || ScreenPosition.X > SizeX || ScreenPosition.Y > SizeY)
		{
			bIsOnScreen = false;
		}
	}

	if (bIsOnScreen)
	{
		const float DPIScale = UWidgetLayoutLibrary::GetViewportScale(LockOnWidget.Get());
		if (DPIScale > 0.0f)
		{
			ScreenPosition /= DPIScale;
		}

		LockOnWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		LockOnWidget->UpdateWidget(ScreenPosition);
	}
	else
	{
		LockOnWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}
