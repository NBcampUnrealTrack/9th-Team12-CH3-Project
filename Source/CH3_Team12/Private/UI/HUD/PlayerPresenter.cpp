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

	AttributeComponent->OnHealthChanged.AddDynamic(this, &UPlayerPresenter::OnModelHealthChanged);
	AttributeComponent->OnPostureChanged.AddDynamic(this, &UPlayerPresenter::OnModelPostureChanged);
	AttributeComponent->OnPostureBroken.AddDynamic(this, &UPlayerPresenter::OnModelPostureBroken);
	AttributeComponent->OnPostureRecovered.AddDynamic(this, &UPlayerPresenter::OnModelPostureRecovered);
	AttributeComponent->OnDeath.AddDynamic(this, &UPlayerPresenter::OnModelDeath);

	OnModelHealthChanged(AttributeComponent->GetCurrentHealth(), AttributeComponent->GetMaxHealth());
	OnModelPostureChanged(AttributeComponent->GetCurrentPosture(), AttributeComponent->GetMaxPosture());
}

void UPlayerPresenter::Dispose()
{
	if (!AttributeComponent.IsValid())
		return;

	AttributeComponent->OnHealthChanged.RemoveDynamic(this, &UPlayerPresenter::OnModelHealthChanged);
	AttributeComponent->OnPostureChanged.RemoveDynamic(this, &UPlayerPresenter::OnModelPostureChanged);
	AttributeComponent->OnPostureBroken.RemoveDynamic(this, &UPlayerPresenter::OnModelPostureBroken);
	AttributeComponent->OnPostureRecovered.RemoveDynamic(this, &UPlayerPresenter::OnModelPostureRecovered);
	AttributeComponent->OnDeath.RemoveDynamic(this, &UPlayerPresenter::OnModelDeath);
}

void UPlayerPresenter::OnModelHealthChanged(const float CurrentHealth, const float MaxHealth) const
{
	if (PlayerWidget.IsValid() && MaxHealth > 0.0f)
	{
		const float Percent = CurrentHealth / MaxHealth;
		PlayerWidget->UpdateHealthBar(Percent);
	}
}

void UPlayerPresenter::OnModelPostureChanged(const float CurrentPosture, const float MaxPosture) const
{
	if (PlayerWidget.IsValid() && MaxPosture > 0.0f)
	{
		const float Percent = CurrentPosture / MaxPosture;
		PlayerWidget->UpdatePostureBar(Percent);
	}
}

void UPlayerPresenter::OnModelPostureBroken() const
{
	// UE_LOG(LogTemp, Warning, TEXT("Posture Broken"));
}

void UPlayerPresenter::OnModelPostureRecovered() const
{
	// UE_LOG(LogTemp, Warning, TEXT("Posture Recovered"));
}

void UPlayerPresenter::OnModelDeath() const
{

}
