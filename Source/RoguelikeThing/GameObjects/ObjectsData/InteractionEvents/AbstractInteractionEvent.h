// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AbstractInteractionEvent.generated.h"

/**
 * 
 */
UCLASS()
class ROGUELIKETHING_API UAbstractInteractionEvent : public UObject
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable)
	virtual void DoThing() PURE_VIRTUAL(UAbstractInteractionEvent::DoThing, );
};
