// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "RoguelikeThing/GameObjects/ObjectsData/InteractionEvents/AbstractInteractionEvent.h"
#include "AddItemEvent.generated.h"

/**
 * 
 */
UCLASS()
class ROGUELIKETHING_API UAddItemEvent : public UAbstractInteractionEvent
{
	GENERATED_BODY()

private:

	FString ItemModule = "";
	FString ItemCategory = "";
	FString ItemSubCategory = "";
	FString ItemID = "";
	int Quantity = 0;

public:
	void Init(FString itemModule, FString itemCategory, FString itemSubCategory, FString itemID, int quantity);

	void DoThing() override;
};
