// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "RoguelikeThing/GameObjects/ObjectsData/InteractionConditions/AbstractInteractionCondition.h"
#include "InteractionConditionUsingItem.generated.h"

/**
 * 
 */

UCLASS()
class ROGUELIKETHING_API UInteractionConditionUsingItem : public UAbstractInteractionCondition
{
	GENERATED_BODY()

private:
	FString ItemModule = "";
	FString ItemCategory = "";
	FString ItemSubCategory = "";
	FString ItemID = "";
	int Quantity = 0;

public:
	UPROPERTY(BlueprintReadWrite)
	FString LackOfQuantityText = "";

	void Init(FString itemModule, FString itemCategory, FString itemSubCategory, FString itemID, int quantity, FString lackOfQuantityText);

	CheckConditionResult CheckCondition() override;
};
