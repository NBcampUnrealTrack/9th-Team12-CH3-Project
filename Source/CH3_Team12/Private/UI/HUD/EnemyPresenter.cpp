#include "UI/HUD/EnemyPresenter.h"

#include "Entity/Enemy/Component/EnemyAttributeComponent.h"
#include "UI/HUD/EnemyWidget.h"

void UEnemyPresenter::Initialize(UEnemyAttributeComponent* InAttributeComponent,
                                       UEnemyWidget* InWidget)
{
	AttributeComponent = InAttributeComponent;
	EnemyWidget = InWidget;

	if (!AttributeComponent.IsValid() || !EnemyWidget.IsValid())
		return;

	AttributeComponent->OnEnemyHealthChanged.AddDynamic(this, &UEnemyPresenter::OnModelHealthChanged);
	AttributeComponent->OnEnemyPostureChanged.AddDynamic(this, &UEnemyPresenter::OnModelPostureChanged);
	AttributeComponent->OnEnemyPostureBroken.AddDynamic(this, &UEnemyPresenter::OnModelPostureBroken);
	AttributeComponent->OnEnemyPostureRecovered.AddDynamic(this, &UEnemyPresenter::OnModelPostureRecovered);
	AttributeComponent->OnEnemyDeath.AddDynamic(this, &UEnemyPresenter::OnModelDeath);

	OnModelHealthChanged(AttributeComponent->GetCurrentHealth(), AttributeComponent->GetMaxHealth());
	OnModelPostureChanged(AttributeComponent->GetCurrentPosture(), AttributeComponent->GetMaxPosture());
}

void UEnemyPresenter::Dispose()
{
	if (!AttributeComponent.IsValid())
		return;

	AttributeComponent->OnEnemyHealthChanged.RemoveDynamic(this, &UEnemyPresenter::OnModelHealthChanged);
	AttributeComponent->OnEnemyPostureChanged.RemoveDynamic(this, &UEnemyPresenter::OnModelPostureChanged);
	AttributeComponent->OnEnemyPostureBroken.RemoveDynamic(this, &UEnemyPresenter::OnModelPostureBroken);
	AttributeComponent->OnEnemyPostureRecovered.RemoveDynamic(this, &UEnemyPresenter::OnModelPostureRecovered);
	AttributeComponent->OnEnemyDeath.RemoveDynamic(this, &UEnemyPresenter::OnModelDeath);
}

void UEnemyPresenter::OnModelHealthChanged(const float CurrentHealth, const float MaxHealth) const
{
	if (EnemyWidget.IsValid() && MaxHealth > 0.0f)
	{
		const float Percent = CurrentHealth / MaxHealth;
		EnemyWidget->UpdateHealthBar(Percent);
	}
}

void UEnemyPresenter::OnModelPostureChanged(const float CurrentPosture, const float MaxPosture) const
{
	if (EnemyWidget.IsValid() && MaxPosture > 0.0f)
	{
		const float Percent = CurrentPosture / MaxPosture;
		EnemyWidget->UpdatePostureBar(Percent);
	}
}

void UEnemyPresenter::OnModelPostureBroken() const
{

}

void UEnemyPresenter::OnModelPostureRecovered() const
{

}

void UEnemyPresenter::OnModelDeath() const
{

}
