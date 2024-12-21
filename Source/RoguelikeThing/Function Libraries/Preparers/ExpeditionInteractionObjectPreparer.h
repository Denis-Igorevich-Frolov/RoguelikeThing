// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RoguelikeThing/GameObjects/ExpeditionInteractionObjectData.h"
#include "ExpeditionInteractionObjectPreparer.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ROGUELIKETHING_API UExpeditionInteractionObjectPreparer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	void CheckingDefaultExpeditionInteractionObjects();
	
public:
	UFUNCTION(BlueprintCallable)
	TMap<FString, UExpeditionInteractionObjectData*> GetAllExpeditionInteractionObjectsData();
};
