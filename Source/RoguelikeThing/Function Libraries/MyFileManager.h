// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyFileManager.generated.h"

/**
 * 
 */
DECLARE_LOG_CATEGORY_EXTERN(MyFileManager, Log, All);

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UMyFileManager : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	bool CopyFileToTemporaryDirectory(FString OriginalFileName, FString PathToDirectoryInsideTemporaryFolder, FString TemporaryFileName);
};
