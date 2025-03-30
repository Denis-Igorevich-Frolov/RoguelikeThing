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
	FString FileTag;
	UPROPERTY(SaveGame)
	TArray<uint8> ObjectsBinArray;
};

UCLASS()
class ROGUELIKETHING_API UFileBinArrayData : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(SaveGame)
	FString ItemName;
	UPROPERTY(SaveGame)
	FFileBinary FileBinary;
};
