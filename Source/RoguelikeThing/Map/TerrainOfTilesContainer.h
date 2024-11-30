// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RoguelikeThing/Map/TerrainOfTile.h"
#include "TerrainOfTilesContainer.generated.h"

/**
 * 
 */

UCLASS()
class ROGUELIKETHING_API UTerrainOfTilesRow : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
	TMap<int, UTerrainOfTile*> TerrainOfTilesRow;
};

UCLASS()
class ROGUELIKETHING_API UTerrainOfTilesContainer : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(SaveGame)
	TMap<int, UTerrainOfTilesRow*> TerrainOfTilesRows;
};

