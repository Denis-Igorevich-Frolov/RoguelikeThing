// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "FileBinArrayData.generated.h"

/**
 * 
 */

USTRUCT()
struct FFileBinary {
	GENERATED_BODY()

	UPROPERTY(SaveGame)
	TArray<uint8> ObjectsBinArray;
};

UCLASS()
class ROGUELIKETHING_API UFileBinArrayData : public UObject
{
	GENERATED_BODY()

public:

	void Init(FString moduleName, FString categoryName, FString subCategoryName, FString itemName, FString textureTag, FFileBinary fileBinary);
	
	UPROPERTY(SaveGame)
	FString ModuleName;
	UPROPERTY(SaveGame)
	FString CategoryName;
	UPROPERTY(SaveGame)
	FString SubCategoryName;
	UPROPERTY(SaveGame)
	FString ItemName;
	UPROPERTY(SaveGame)
	FString TextureTag;

	UPROPERTY(SaveGame)
	FFileBinary FileBinary;
};
