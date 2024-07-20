// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "CellCoord.generated.h"

/***********************************************************************
 * Данная структура является труктурой глобальной координаты клетки
 ***********************************************************************/
USTRUCT(BlueprintType)
struct FCellCoord
{
public:
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Row = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Col = -1;

	FCellCoord(int32 Row = -1, int32 Col = -1);

	bool operator == (const FCellCoord& Coord) const;
	bool operator != (const FCellCoord& Coord) const;
};