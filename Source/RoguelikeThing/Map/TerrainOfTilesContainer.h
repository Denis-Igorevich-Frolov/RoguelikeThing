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
class ROGUELIKETHING_API UTerrainOfTilesContainer : public UObject
{
	GENERATED_BODY()
	
public:
	TMap<int, TMap<int, UTerrainOfTile*>> TerrainOfTilesRows;
};
