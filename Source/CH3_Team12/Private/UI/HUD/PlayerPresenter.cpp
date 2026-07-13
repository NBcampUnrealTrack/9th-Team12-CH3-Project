#include "UI/HUD/PlayerPresenter.h"

#include "Entity/Player/PlayerAttributeComponent.h"
#include "UI/HUD/PlayerWidget.h"

void UPlayerPresenter::Initialize(UPlayerAttributeComponent* InAttributeComponent,
                                  UPlayerWidget* InWidget)
{
	AttributeComponent = InAttributeComponent;
	PlayerWidget = InWidget;

	if (!AttributeComponent.IsValid() || !PlayerWidget.IsValid())
		return;

	AttributeComponent->OnHealthChanged.AddDynamic(this, &UPlayerPresenter::HandleModelHealthChanged);
	AttributeComponent->OnPostureChanged.AddDynamic(this, &UPlayerPresenter::HandleModelPostureChanged);
	AttributeComponent->OnPostureBroken.AddDynamic(this, &UPlayerPresenter::HandleModelPostureBroken);
	AttributeComponent->OnPostureRecovered.AddDynamic(this, &UPlayerPresenter::HandleModelPostureRecovered);
	AttributeComponent->OnDead.AddDynamic(this, &UPlayerPresenter::HandleModelDeath);

	HandleModelHealthChanged(AttributeComponent->GetCurrentHealth(), AttributeComponent->GetMaxHealth());
	HandleModelPostureChanged(AttributeComponent->GetCurrentPosture(), AttributeComponent->GetMaxPosture());
}

void UPlayerPresenter::Dispose()
{
	if (!AttributeComponent.IsValid())
		return;

	AttributeComponent->OnHealthChanged.RemoveDynamic(this, &UPlayerPresenter::HandleModelHealthChanged);
	AttributeComponent->OnPostureChanged.RemoveDynamic(this, &UPlayerPresenter::HandleModelPostureChanged);
	AttributeComponent->OnPostureBroken.RemoveDynamic(this, &UPlayerPresenter::HandleModelPostureBroken);
	AttributeComponent->OnPostureRecovered.RemoveDynamic(this, &UPlayerPresenter::HandleModelPostureRecovered);
	AttributeComponent->OnDead.RemoveDynamic(this, &UPlayerPresenter::HandleModelDeath);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UPlayerPresenter::HandleModelHealthChanged(const float CurrentHealth, const float MaxHealth)
{
	if (PlayerWidget.IsValid() && MaxHealth > 0.0f)
	{
		const float Percent = CurrentHealth / MaxHealth;
		PlayerWidget->UpdateHealthBar(Percent);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UPlayerPresenter::HandleModelPostureChanged(const float CurrentPosture, const float MaxPosture)
{
	if (PlayerWidget.IsValid() && MaxPosture > 0.0f)
	{
		const float Percent = CurrentPosture / MaxPosture;
		PlayerWidget->UpdatePostureBar(Percent);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UPlayerPresenter::HandleModelPostureBroken()
{
	// UE_LOG(LogTemp, Warning, TEXT("Posture Broken"));
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UPlayerPresenter::HandleModelPostureRecovered()
{
	// UE_LOG(LogTemp, Warning, TEXT("Posture Recovered"));
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UPlayerPresenter::HandleModelDeath()
{

}
