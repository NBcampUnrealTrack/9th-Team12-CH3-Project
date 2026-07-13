#include "Entity/Player/PlayerDebugOverlayComponent.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Entity/Player/StateTagComponent.h"
#include "Entity/Player/PlayerAttributeComponent.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "GameplayTagContainer.h"

UPlayerDebugOverlayComponent::UPlayerDebugOverlayComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UPlayerDebugOverlayComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<APlayerCharacterBase>(GetOwner());
	if (!OwnerCharacter)
	{
		return;
	}

	StateComponent = OwnerCharacter->GetStateTagComponent();
	AttributeComponent = OwnerCharacter->GetAttributeComponent();
	MovementComponent = OwnerCharacter->GetCharacterMovement();
}

void UPlayerDebugOverlayComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bShowDebugOverlay || !GEngine)
	{
		return;
	}

	GEngine->AddOnScreenDebugMessage(
		ScreenMessageKey,
		0.0f,
		FColor::Cyan,
		MakeDebugText()
	);
}

FString UPlayerDebugOverlayComponent::MakeDebugText() const
{
	FString Text;

	Text += TEXT("==== PLAYER DEBUG ====\n");

	if (StateComponent)
	{
		Text += TEXT("[State Tags]\n");

		const FGameplayTagContainer Tags =
			StateComponent->GetStateTagsCopy();

		TArray<FGameplayTag> TagArray;
		Tags.GetGameplayTagArray(TagArray);

		TagArray.Sort([](const FGameplayTag& A, const FGameplayTag& B)
		{
			return A.ToString() < B.ToString();
		});

		if (TagArray.Num() == 0)
		{
			Text += TEXT("None\n");
		}
		else
		{
			for (const FGameplayTag& Tag : TagArray)
			{
				Text += FString::Printf(
					TEXT("- %s\n"),
					*Tag.ToString()
				);
			}
		}
	}

	if (AttributeComponent)
	{
		Text += TEXT("\n[Attribute]\n");

		Text += FString::Printf(
			TEXT("Dead: %s\n"),
			AttributeComponent->IsDead() ? TEXT("true") : TEXT("false")
		);

		Text += FString::Printf(
			TEXT("HP: %.1f\n"),
			AttributeComponent->GetCurrentHealth()
		);
		
		Text += FString::Printf(
			TEXT("Posture: %.1f\n"),
			AttributeComponent->GetCurrentPosture()
		);
	}

	if (MovementComponent)
	{
		Text += TEXT("\n[Movement]\n");

		Text += FString::Printf(
			TEXT("Velocity: %.1f\n"),
			MovementComponent->Velocity.Size2D()
		);

		Text += FString::Printf(
			TEXT("MaxWalkSpeed: %.1f\n"),
			MovementComponent->MaxWalkSpeed
		);

		Text += FString::Printf(
			TEXT("OrientToMovement: %s\n"),
			MovementComponent->bOrientRotationToMovement ? TEXT("true") : TEXT("false")
		);

		Text += FString::Printf(
			TEXT("UseControllerDesiredRotation: %s\n"),
			MovementComponent->bUseControllerDesiredRotation ? TEXT("true") : TEXT("false")
		);
	}

	return Text;
}
