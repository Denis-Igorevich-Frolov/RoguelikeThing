// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RoguelikeThing/GameObjects/ExpeditionInteractionObjectData.h"
#include "RoguelikeThing/Lists/ExpeditionInteractionObjectsList.h"
#include "ExpeditionInteractionObjectPreparer.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ROGUELIKETHING_API UExpeditionInteractionObjectPreparer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:

	ExpeditionInteractionObjectsList expeditionInteractionObjectsList;

	void CheckingDefaultExpeditionInteractionObjects();
	UExpeditionInteractionObjectData* LoadExpeditionInteractionObject(FString ModuleName, FString XMLFilePath, FString ModuleSAVDir, IPlatformFile& FileManager);
	
public:
	UFUNCTION(BlueprintCallable)
	void RestoringDefaultFileByName(FString Name);

	UFUNCTION(BlueprintCallable)
	void GetAllExpeditionInteractionObjectsData(UPARAM(ref)TMap<FString, UExpeditionInteractionObjectData*>& InteractionObjectsDataArray);
};
