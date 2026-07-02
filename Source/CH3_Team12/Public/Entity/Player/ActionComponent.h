#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActionComponent.generated.h"

class UAnimMontage;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CH3_TEAM12_API UActionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UActionComponent();

protected:
	virtual void BeginPlay() override;
	
public:
	void DoJump();
	void DoDodge();
	
protected:
	UPROPERTY(EditAnywhere, Category = "Action|Montages")
	TObjectPtr<UAnimMontage> DodgeMontage;
	
};
