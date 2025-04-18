// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "RoguelikeThing/GameObjects/ObjectsData/InteractionEvents/AbstractInteractionEvent.h"
#include "MakeObjectUsedEvent.generated.h"

/**
 * 
 */
UCLASS()
class ROGUELIKETHING_API UMakeObjectUsedEvent : public UAbstractInteractionEvent
{
	GENERATED_BODY()

public:
	
	void DoThing() override;
};
