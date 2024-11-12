// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RoguelikeThing/Map/TerrainOfTilesContainer.h"
#include "RoguelikeThing/Widgets/MapEditor/CellCoord.h"
#include "MySaveGame.generated.h"

/**
 * 
 */
UCLASS()
class ROGUELIKETHING_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere)
	UTerrainOfTilesContainer* TerrainOfTilesContainer;

	UPROPERTY(VisibleAnywhere)
	FCellCoord MinNoEmptyTileCoord = FCellCoord(-1, -1);

	UPROPERTY(VisibleAnywhere)
	FCellCoord MaxNoEmptyTileCoord = FCellCoord(-1, -1);

	UPROPERTY(VisibleAnywhere)
	FString MapDataBaseHex;
};
