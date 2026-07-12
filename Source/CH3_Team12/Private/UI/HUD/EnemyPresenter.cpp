#include "UI/HUD/EnemyPresenter.h"

#include "Entity/Enemy/Component/EnemyAttributeComponent.h"
#include "UI/HUD/EnemyWidget.h"

void UEnemyPresenter::Initialize(const FString& InName, UEnemyAttributeComponent* InAttributeComponent,
                                       UEnemyWidget* InWidget)
{
	AttributeComponent = InAttributeComponent;
	EnemyWidget = InWidget;

	if (!AttributeComponent.IsValid() || !EnemyWidget.IsValid())
		return;

	AttributeComponent->OnEnemyHealthChanged.AddDynamic(this, &UEnemyPresenter::HandleModelHealthChanged);
	AttributeComponent->OnEnemyPostureChanged.AddDynamic(this, &UEnemyPresenter::HandleModelPostureChanged);
	AttributeComponent->OnEnemyPostureBroken.AddDynamic(this, &UEnemyPresenter::HandleModelPostureBroken);
	AttributeComponent->OnEnemyPostureRecovered.AddDynamic(this, &UEnemyPresenter::HandleModelPostureRecovered);
	AttributeComponent->OnEnemyDeath.AddDynamic(this, &UEnemyPresenter::HandleModelDeath);

	EnemyWidget->UpdateName(InName);
	HandleModelHealthChanged(AttributeComponent->GetCurrentHealth(), AttributeComponent->GetMaxHealth());
	HandleModelPostureChanged(AttributeComponent->GetCurrentPosture(), AttributeComponent->GetMaxPosture());
}

void UEnemyPresenter::Dispose()
{
	if (!AttributeComponent.IsValid())
		return;

	AttributeComponent->OnEnemyHealthChanged.RemoveDynamic(this, &UEnemyPresenter::HandleModelHealthChanged);
	AttributeComponent->OnEnemyPostureChanged.RemoveDynamic(this, &UEnemyPresenter::HandleModelPostureChanged);
	AttributeComponent->OnEnemyPostureBroken.RemoveDynamic(this, &UEnemyPresenter::HandleModelPostureBroken);
	AttributeComponent->OnEnemyPostureRecovered.RemoveDynamic(this, &UEnemyPresenter::HandleModelPostureRecovered);
	AttributeComponent->OnEnemyDeath.RemoveDynamic(this, &UEnemyPresenter::HandleModelDeath);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UEnemyPresenter::HandleModelHealthChanged(const float CurrentHealth, const float MaxHealth)
{
	if (EnemyWidget.IsValid() && MaxHealth > 0.0f)
	{
		const float Percent = CurrentHealth / MaxHealth;
		EnemyWidget->UpdateHealthBar(Percent);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UEnemyPresenter::HandleModelPostureChanged(const float CurrentPosture, const float MaxPosture)
{
	if (EnemyWidget.IsValid() && MaxPosture > 0.0f)
	{
		const float Percent = CurrentPosture / MaxPosture;
		EnemyWidget->UpdatePostureBar(Percent);
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UEnemyPresenter::HandleModelPostureBroken()
{

}

// ReSharper disable once CppMemberFunctionMayBeConst
void UEnemyPresenter::HandleModelPostureRecovered()
{

}

// ReSharper disable once CppMemberFunctionMayBeConst
void UEnemyPresenter::HandleModelDeath()
{

}
