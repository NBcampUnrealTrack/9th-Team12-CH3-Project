#include "UI/KatanaEnemyPresenter.h"

#include "UI/KatanaEnemyWidget.h"

void UKatanaEnemyPresenter::Initialize(UObject* InAttributeComponent,
                                          UKatanaEnemyWidget* InWidget)
{
	AttributeComponent = InAttributeComponent;
	EnemyWidget = InWidget;

	if (!AttributeComponent.IsValid() || !EnemyWidget.IsValid())
		return;

	//TODO 이름 설정

	// AttributeComponent->OnHealthChanged.AddDynamic(this, &UKatanaEnemyPresenter::OnModelHealthChanged);
	// AttributeComponent->OnPostureChanged.AddDynamic(this, &UKatanaEnemyPresenter::OnModelPostureChanged);
	// AttributeComponent->OnPostureBroken.AddDynamic(this, &UKatanaEnemyPresenter::OnModelPostureBroken);
	// AttributeComponent->OnPostureRecovered.AddDynamic(this, &UKatanaEnemyPresenter::OnModelPostureRecovered);
	// AttributeComponent->OnDeath.AddDynamic(this, &UKatanaEnemyPresenter::OnModelDeath);
	//
	// OnModelHealthChanged(AttributeComponent->GetCurrentHealth(), AttributeComponent->GetMaxHealth());
	// OnModelPostureChanged(AttributeComponent->GetCurrentPosture(), AttributeComponent->GetMaxPosture());
}

void UKatanaEnemyPresenter::Dispose()
{
	if (!AttributeComponent.IsValid())
		return;

	// AttributeComponent->OnHealthChanged.RemoveDynamic(this, &UKatanaEnemyPresenter::OnModelHealthChanged);
	// AttributeComponent->OnPostureChanged.RemoveDynamic(this, &UKatanaEnemyPresenter::OnModelPostureChanged);
	// AttributeComponent->OnPostureBroken.RemoveDynamic(this, &UKatanaEnemyPresenter::OnModelPostureBroken);
	// AttributeComponent->OnPostureRecovered.RemoveDynamic(this, &UKatanaEnemyPresenter::OnModelPostureRecovered);
	// AttributeComponent->OnDeath.RemoveDynamic(this, &UKatanaEnemyPresenter::OnModelDeath);
}

void UKatanaEnemyPresenter::OnModelHealthChanged(const float CurrentHealth, const float MaxHealth) const
{
	if (EnemyWidget.IsValid() && MaxHealth > 0.0f)
	{
		const float Percent = CurrentHealth / MaxHealth;
		EnemyWidget->UpdateHealthBar(Percent);
	}
}

void UKatanaEnemyPresenter::OnModelPostureChanged(const float CurrentPosture, const float MaxPosture) const
{
	if (EnemyWidget.IsValid() && MaxPosture > 0.0f)
	{
		const float Percent = CurrentPosture / MaxPosture;
		EnemyWidget->UpdatePostureBar(Percent);
	}
}

void UKatanaEnemyPresenter::OnModelPostureBroken() const
{

}

void UKatanaEnemyPresenter::OnModelPostureRecovered() const
{

}

void UKatanaEnemyPresenter::OnModelDeath() const
{

}
