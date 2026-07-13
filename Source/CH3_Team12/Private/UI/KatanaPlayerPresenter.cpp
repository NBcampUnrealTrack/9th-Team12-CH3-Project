#include "UI/KatanaPlayerPresenter.h"

#include "Entity/Player/PlayerAttributeComponent.h"
#include "UI/KatanaPlayerWidget.h"

void UKatanaPlayerPresenter::Initialize(UPlayerAttributeComponent* InAttributeComponent,
                                          UKatanaPlayerWidget* InWidget)
{
	AttributeComponent = InAttributeComponent;
	PlayerWidget = InWidget;

	if (!AttributeComponent.IsValid() || !PlayerWidget.IsValid())
		return;

	AttributeComponent->OnHealthChanged.AddDynamic(this, &UKatanaPlayerPresenter::OnModelHealthChanged);
	AttributeComponent->OnPostureChanged.AddDynamic(this, &UKatanaPlayerPresenter::OnModelPostureChanged);
	AttributeComponent->OnPostureBroken.AddDynamic(this, &UKatanaPlayerPresenter::OnModelPostureBroken);
	AttributeComponent->OnPostureRecovered.AddDynamic(this, &UKatanaPlayerPresenter::OnModelPostureRecovered);
	AttributeComponent->OnDead.AddDynamic(this, &UKatanaPlayerPresenter::OnModelDeath);

	OnModelHealthChanged(AttributeComponent->GetCurrentHealth(), AttributeComponent->GetMaxHealth());
	OnModelPostureChanged(AttributeComponent->GetCurrentPosture(), AttributeComponent->GetMaxPosture());
}

void UKatanaPlayerPresenter::Dispose()
{
	if (!AttributeComponent.IsValid())
		return;

	AttributeComponent->OnHealthChanged.RemoveDynamic(this, &UKatanaPlayerPresenter::OnModelHealthChanged);
	AttributeComponent->OnPostureChanged.RemoveDynamic(this, &UKatanaPlayerPresenter::OnModelPostureChanged);
	AttributeComponent->OnPostureBroken.RemoveDynamic(this, &UKatanaPlayerPresenter::OnModelPostureBroken);
	AttributeComponent->OnPostureRecovered.RemoveDynamic(this, &UKatanaPlayerPresenter::OnModelPostureRecovered);
	AttributeComponent->OnDead.RemoveDynamic(this, &UKatanaPlayerPresenter::OnModelDeath);
}

void UKatanaPlayerPresenter::OnModelHealthChanged(const float CurrentHealth, const float MaxHealth) const
{
	if (PlayerWidget.IsValid() && MaxHealth > 0.0f)
	{
		const float Percent = CurrentHealth / MaxHealth;
		PlayerWidget->UpdateHealthBar(Percent);
	}
}

void UKatanaPlayerPresenter::OnModelPostureChanged(const float CurrentPosture, const float MaxPosture) const
{
	if (PlayerWidget.IsValid() && MaxPosture > 0.0f)
	{
		const float Percent = CurrentPosture / MaxPosture;
		PlayerWidget->UpdatePostureBar(Percent);
	}
}

void UKatanaPlayerPresenter::OnModelPostureBroken() const
{
	// UE_LOG(LogTemp, Warning, TEXT("Posture Broken"));
}

void UKatanaPlayerPresenter::OnModelPostureRecovered() const
{
	// UE_LOG(LogTemp, Warning, TEXT("Posture Recovered"));
}

void UKatanaPlayerPresenter::OnModelDeath() const
{

}
