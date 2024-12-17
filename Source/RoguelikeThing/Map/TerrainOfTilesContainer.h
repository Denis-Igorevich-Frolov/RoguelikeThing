// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RoguelikeThing/Map/TerrainOfTile.h"
#include "RoguelikeThing/Enumerations/CellType.h"
#include "MapDimensions.h"
#include "TerrainOfTilesContainer.generated.h"

/**
 * 
 */

UCLASS()
class ROGUELIKETHING_API UTerrainOfTilesRow : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<int, UTerrainOfTile*> TerrainOfTilesRow;
};

UCLASS()
class ROGUELIKETHING_API UTerrainOfTilesContainer : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<int, UTerrainOfTilesRow*> TerrainOfTilesRows;

	UPROPERTY(SaveGame)
	TMap<FVector2D, FCellType> ReCreationContainer;

	UFUNCTION(BlueprintCallable)
	void ReCreateTerrains(FMapDimensions MapDimensions);
};

