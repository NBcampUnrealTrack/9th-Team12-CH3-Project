#include "UI/KatanaPlayerUIPresenter.h"

#include "Entity/Player/PlayerAttributeComponent.h"
#include "UI/KatanaPlayerWidget.h"

void UKatanaPlayerUIPresenter::Initialize(UPlayerAttributeComponent* InAttributeComponent,
                                          UKatanaPlayerWidget* InWidget)
{
	AttributeComponent = InAttributeComponent;
	PlayerWidget = InWidget;

	if (!AttributeComponent.IsValid() || !PlayerWidget.IsValid())
		return;

	AttributeComponent->OnHealthChanged.AddDynamic(this, &UKatanaPlayerUIPresenter::OnModelHealthChanged);
	AttributeComponent->OnPostureChanged.AddDynamic(this, &UKatanaPlayerUIPresenter::OnModelPostureChanged);
	AttributeComponent->OnPostureBroken.AddDynamic(this, &UKatanaPlayerUIPresenter::OnModelPostureBroken);
	AttributeComponent->OnPostureRecovered.AddDynamic(this, &UKatanaPlayerUIPresenter::OnModelPostureRecovered);
	AttributeComponent->OnDeath.AddDynamic(this, &UKatanaPlayerUIPresenter::OnModelDeath);

	OnModelHealthChanged(AttributeComponent->GetCurrentHealth(), AttributeComponent->GetMaxHealth());
	OnModelPostureChanged(AttributeComponent->GetCurrentPosture(), AttributeComponent->GetMaxPosture());
}

void UKatanaPlayerUIPresenter::Dispose()
{
	if (!AttributeComponent.IsValid())
		return;

	AttributeComponent->OnHealthChanged.RemoveDynamic(this, &UKatanaPlayerUIPresenter::OnModelHealthChanged);
	AttributeComponent->OnPostureChanged.RemoveDynamic(this, &UKatanaPlayerUIPresenter::OnModelPostureChanged);
	AttributeComponent->OnPostureBroken.RemoveDynamic(this, &UKatanaPlayerUIPresenter::OnModelPostureBroken);
	AttributeComponent->OnPostureRecovered.RemoveDynamic(this, &UKatanaPlayerUIPresenter::OnModelPostureRecovered);
	AttributeComponent->OnDeath.RemoveDynamic(this, &UKatanaPlayerUIPresenter::OnModelDeath);
}

void UKatanaPlayerUIPresenter::OnModelHealthChanged(const float CurrentHealth, const float MaxHealth) const
{
	if (PlayerWidget.IsValid() && MaxHealth > 0.0f)
	{
		const float Percent = CurrentHealth / MaxHealth;
		PlayerWidget->UpdateHealthBar(Percent);
	}
}

void UKatanaPlayerUIPresenter::OnModelPostureChanged(const float CurrentPosture, const float MaxPosture) const
{
	if (PlayerWidget.IsValid() && MaxPosture > 0.0f)
	{
		const float Percent = CurrentPosture / MaxPosture;
		PlayerWidget->UpdatePostureBar(Percent);
	}
}

void UKatanaPlayerUIPresenter::OnModelPostureBroken() const
{

}

void UKatanaPlayerUIPresenter::OnModelPostureRecovered() const
{

}

void UKatanaPlayerUIPresenter::OnModelDeath() const
{

}
