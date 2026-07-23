// ReSharper disable CppMemberFunctionMayBeConst
#include "UI/HUD/EnemyPresenter.h"

#include "Entity/Enemy/EnemyCharacterBase.h"
#include "Entity/Enemy/Component/EnemyAttributeComponent.h"
#include "Framework/Subsystem/KatanaUIManagerSubsystem.h"
#include "UI/HUD/EnemyWidget.h"

void UEnemyPresenter::Initialize(const AEnemyCharacterBase* InEnemyCharacterBase, UEnemyWidget* InWidget)
{
	if (!InEnemyCharacterBase || !InWidget)
		return;

	EnemyWidget = InWidget;
	EnemyWidget->UpdateName(InEnemyCharacterBase->GetEnemyName());

	AttributeComponent = InEnemyCharacterBase->GetEnemyAttributeComponent();

	if (AttributeComponent.IsValid())
	{
		AttributeComponent->OnEnemyHealthChanged.AddDynamic(this, &UEnemyPresenter::HandleModelHealthChanged);
		AttributeComponent->OnEnemyPostureChanged.AddDynamic(this, &UEnemyPresenter::HandleModelPostureChanged);
		AttributeComponent->OnEnemyPostureBroken.AddDynamic(this, &UEnemyPresenter::HandleModelPostureBroken);
		AttributeComponent->OnEnemyPostureRecovered.AddDynamic(this, &UEnemyPresenter::HandleModelPostureRecovered);
		AttributeComponent->OnEnemyDeath.AddDynamic(this, &UEnemyPresenter::HandleModelDeath);

		HandleModelHealthChanged(AttributeComponent->GetCurrentHealth(), AttributeComponent->GetMaxHealth());
		HandleModelPostureChanged(AttributeComponent->GetCurrentPosture(), AttributeComponent->GetMaxPosture());
	}
}

void UEnemyPresenter::Dispose()
{
	if (AttributeComponent.IsValid())
	{
		AttributeComponent->OnEnemyHealthChanged.RemoveDynamic(this, &UEnemyPresenter::HandleModelHealthChanged);
		AttributeComponent->OnEnemyPostureChanged.RemoveDynamic(this, &UEnemyPresenter::HandleModelPostureChanged);
		AttributeComponent->OnEnemyPostureBroken.RemoveDynamic(this, &UEnemyPresenter::HandleModelPostureBroken);
		AttributeComponent->OnEnemyPostureRecovered.RemoveDynamic(this, &UEnemyPresenter::HandleModelPostureRecovered);
		AttributeComponent->OnEnemyDeath.RemoveDynamic(this, &UEnemyPresenter::HandleModelDeath);
	}
}

void UEnemyPresenter::HandleModelHealthChanged(const float CurrentHealth, const float MaxHealth)
{
	if (EnemyWidget.IsValid() && MaxHealth > 0.0f)
	{
		const float Percent = CurrentHealth / MaxHealth;
		EnemyWidget->UpdateHealthBar(Percent);
	}
}

void UEnemyPresenter::HandleModelPostureChanged(const float CurrentPosture, const float MaxPosture)
{
	if (EnemyWidget.IsValid() && MaxPosture > 0.0f)
	{
		const float Percent = CurrentPosture / MaxPosture;
		EnemyWidget->UpdatePostureBar(Percent);
	}
}

void UEnemyPresenter::HandleModelPostureBroken()
{
	if (EnemyWidget.IsValid())
	{
		EnemyWidget->BreakPostureBar();
	}
}

void UEnemyPresenter::HandleModelPostureRecovered()
{
	if (EnemyWidget.IsValid())
	{
		EnemyWidget->RecoverPostureBar();
	}
}

void UEnemyPresenter::HandleModelDeath()
{
	UKatanaUIManagerSubsystem* UIManager = UKatanaUIManagerSubsystem::Get(this);
	if (!UIManager)
		return;

	UIManager->HideEnemyWidget();
}
