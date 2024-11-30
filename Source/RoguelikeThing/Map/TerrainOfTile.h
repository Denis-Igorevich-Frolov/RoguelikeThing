// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <RoguelikeThing/Widgets/MapEditor/CellCoord.h>
#include "RoguelikeThing/Enumerations/CellType.h"
#include "TerrainOfTile.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(TerrainOfTile, Log, All);

/**
 * 
 */
USTRUCT(BlueprintType)
struct FTerrainOfTileRows
{
	GENERATED_BODY()

public:
	TMap<int, TMap<int, FCellType>> TerrainOfTileRows;
};

 // ласс предзагрузки структуры тайла. ’ранит в себе все не нулевые €чейки тайла
UCLASS(BlueprintType)
class ROGUELIKETHING_API UTerrainOfTile : public UObject
{
	GENERATED_BODY()

private:
	UPROPERTY(SaveGame)
	FTerrainOfTileRows TerrainOfTileRows;

public:
	UFUNCTION(BlueprintCallable)
	void AddCellType(FCellCoord Coord, FCellType CellType);
	UFUNCTION(BlueprintCallable)
	FCellType GetCellType(FCellCoord Coord);
	UFUNCTION(BlueprintCallable)
	bool Contains(FCellCoord Coord);
	//ѕолучение массива всех не нулевых €чеек тайла
	UFUNCTION(BlueprintCallable)
	TArray<FCellCoord> GetFilledCoord();
	UFUNCTION(BlueprintCallable)
	bool RemoveCell(FCellCoord Coord);
};