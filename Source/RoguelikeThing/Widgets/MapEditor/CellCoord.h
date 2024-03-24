// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "CellCoord.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FCellCoord
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Row = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Coll = -1;

	FCellCoord(int32 Row = -1, int32 Coll = -1);
};