#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PresenterInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPresenterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class CH3_TEAM12_API IPresenterInterface
{
	GENERATED_BODY()

public:
	virtual void Dispose() = 0;
};
