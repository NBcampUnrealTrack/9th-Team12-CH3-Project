#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActionComponent.generated.h"

class ACharacter;
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
	void DoMove();
	void DoSprint();
	
protected:
	UPROPERTY(EditAnywhere, Category = "Action|Montages")
	TObjectPtr<UAnimMontage> DodgeMontage;
	
private:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;
};
