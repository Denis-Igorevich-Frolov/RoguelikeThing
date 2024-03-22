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

private:
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	
public:
	UFUNCTION(BlueprintCallable)
	bool CopyFileToTemporaryDirectory(FString PathToOriginalFile, FString PathToDirectoryInsideTempFolder, FString TemporaryFileName);

	UFUNCTION(BlueprintCallable)
	bool SaveTempFileToOriginalDirectory(FString PathToOriginalFile, FString PathToTempFolder);
};
