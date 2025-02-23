// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ModsPreparer.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class ROGUELIKETHING_API UModsPreparer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	TArray<FString> GetArrayOfModDirectories(FString CategoryName);
	
public:
	UFUNCTION(BlueprintCallable)
	TArray<FString> GetArrayOfModDirectoriesHavingExpeditionInteractionObjects();
	UFUNCTION(BlueprintCallable)
	TArray<FString> GetArrayOfModDirectoriesHavingInventoryItems();
};
