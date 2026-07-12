#include "UI/InventoryPresenter.h"

#include "Engine/World.h"
#include "Entity/Player/PlayerCharacterBase.h"
#include "Framework/GameMode/KatanaPlayerController.h"
#include "GameFramework/PlayerController.h"

void UInventoryPresenter::Initialize(UInventoryWidget* InWidget)
{
	if (!GetWorld())
		return;

	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
		return;

	const AKatanaPlayerController* KatanaPlayerController = Cast<AKatanaPlayerController>(PlayerController);
	if (!KatanaPlayerController)
		return;

	const APlayerCharacterBase* PlayerCharacter = KatanaPlayerController->GetPlayerCharacter();
	if (!PlayerCharacter)
		return;

	const UPlayerInventoryComponent* PlayerInventoryComponent = PlayerCharacter->GetInventoryComponent();
	if (!PlayerInventoryComponent)
		return;


}

void UInventoryPresenter::Dispose()
{
}
