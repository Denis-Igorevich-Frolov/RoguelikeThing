// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbstractInteractionCondition.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ROGUELIKETHING_API UAbstractInteractionCondition : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	virtual bool CheckCondition();
};
