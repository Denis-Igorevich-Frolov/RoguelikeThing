// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/SaveGame/MapSaver.h"
#include <Serialization/ObjectAndNameAsStringProxyArchive.h>

//Функция, сериализующая переданную переменную terrainOfTilesContainer
void UMapSaver::SaveTerrainOfTilesContainer(UTerrainOfTilesContainer* terrainOfTilesContainer)
{
    TerrainOfTilesContainer = terrainOfTilesContainer;

    FMemoryWriter TerrainWriter = FMemoryWriter(BinTerrainOfTilesContainer);
    FObjectAndNameAsStringProxyArchive TerrainAr(TerrainWriter, false);
    TerrainAr.ArIsSaveGame = true;
    TerrainOfTilesContainer->Serialize(TerrainAr);
}

//Функция, десериализующая данные из BinTerrainOfTilesContainer
UTerrainOfTilesContainer* UMapSaver::LoadTerrainOfTilesContainer()
{
    FMemoryReader TerrainReader = FMemoryReader(BinTerrainOfTilesContainer);
    FObjectAndNameAsStringProxyArchive TerrainAr(TerrainReader, false);
    TerrainAr.ArIsSaveGame = true;
    TerrainOfTilesContainer = NewObject<UTerrainOfTilesContainer>();
    TerrainOfTilesContainer->Serialize(TerrainAr);
    //Производится восстановление состояния переменных предзагрузки, а за одно актуализируются пременные MinNoEmptyTileCoord и MaxNoEmptyTileCoord
    TerrainOfTilesContainer->ReCreateTerrains(MapDimensions, MinNoEmptyTileCoord, MaxNoEmptyTileCoord);

    return TerrainOfTilesContainer;
}
