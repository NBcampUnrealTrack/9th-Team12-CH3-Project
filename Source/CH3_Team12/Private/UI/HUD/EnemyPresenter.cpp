#include "UI/HUD/EnemyPresenter.h"

#include "UI/HUD/EnemyWidget.h"

void UEnemyPresenter::Initialize(UObject* InAttributeComponent,
                                       UEnemyWidget* InWidget)
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

void UEnemyPresenter::Dispose()
{
	if (!AttributeComponent.IsValid())
		return;

	// AttributeComponent->OnHealthChanged.RemoveDynamic(this, &UKatanaEnemyPresenter::OnModelHealthChanged);
	// AttributeComponent->OnPostureChanged.RemoveDynamic(this, &UKatanaEnemyPresenter::OnModelPostureChanged);
	// AttributeComponent->OnPostureBroken.RemoveDynamic(this, &UKatanaEnemyPresenter::OnModelPostureBroken);
	// AttributeComponent->OnPostureRecovered.RemoveDynamic(this, &UKatanaEnemyPresenter::OnModelPostureRecovered);
	// AttributeComponent->OnDeath.RemoveDynamic(this, &UKatanaEnemyPresenter::OnModelDeath);
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
