// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/SaveGame/MySaveGame.h"
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>

void UMySaveGame::SaveTerrainOfTilesContainer(UTerrainOfTilesContainer* terrainOfTilesContainer)
{
    TerrainOfTilesContainer = terrainOfTilesContainer;

    FMemoryWriter TerrainWriter = FMemoryWriter(BinTerrainOfTilesContainer);
    FObjectAndNameAsStringProxyArchive TerrainAr(TerrainWriter, false);
    TerrainAr.ArIsSaveGame = true;
    TerrainOfTilesContainer->Serialize(TerrainAr);
}

UTerrainOfTilesContainer* UMySaveGame::LoadTerrainOfTilesContainer()
{
    FMemoryReader TerrainReader = FMemoryReader(BinTerrainOfTilesContainer);
    FObjectAndNameAsStringProxyArchive TerrainAr(TerrainReader, false);
    TerrainAr.ArIsSaveGame = true;
    TerrainOfTilesContainer = NewObject<UTerrainOfTilesContainer>();
    TerrainOfTilesContainer->Serialize(TerrainAr);
    TerrainOfTilesContainer->ReCreateTerrains(MapDimensions);

    return TerrainOfTilesContainer;
}
