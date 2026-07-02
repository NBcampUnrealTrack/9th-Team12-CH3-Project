#include "Entity/Player/ActionComponent.h"

UActionComponent::UActionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UActionComponent::BeginPlay()
{
	Super::BeginPlay();
	
	
}
