// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Map/TileBuffer.h"
#include <RoguelikeThing/Widgets/MapEditor/MapTile.h>

DEFINE_LOG_CATEGORY(TileBuffer);

UUserWidget* UTileBuffer::GetTile()
{
    if (!IsInit)
        return false;

    if(TileBuf.Num() == 0)
        return nullptr;

    UUserWidget* Tile = TileBuf.Last();

    if (TileBuf.IsValidIndex(TileBuf.Num() - 1)) {
        TileBuf.RemoveAt(TileBuf.Num() - 1);
    }

    if (TileBuf.Num() < 5) {
        if(MapTileClass)
            ScoreToMaximum();
    }

    UE_LOG(TileBuffer, Warning, TEXT("ssssssss %d"), TileBuf.Num());

    return Tile;
}

void UTileBuffer::Init(int mapTileLength, UUniformGridPanel* tilesGridPanel, UClass* mapTileClass, UClass* cellClass, UMapEditor* mapEditor)
{
    this->MapTileLength = mapTileLength;
    this->TilesGridPanel = tilesGridPanel;
    this->MapTileClass = mapTileClass;
    this->CellClass = cellClass;
    this->MapEditor = mapEditor;

    IsInit = true;

    ScoreToMaximum();
}

bool UTileBuffer::ScoreToMaximum()
{
    if (!IsInit)
        return false;

    if (!TilesGridPanel)
        return false;

    int dif = MaxSize - TileBuf.Num();

    for (int i = 0; i < dif; i++) {
        if (TileBuf.Num() == 30)
            return true;

        UMapTile* MapTile = StaticCast<UMapTile*>(CreateWidget<UUserWidget>(TilesGridPanel, MapTileClass));
        if (MapTile->FillingWithCells(MapTileLength, CellClass, MapEditor))
            return false;

        TileBuf.Add(MapTile);
    }

    return true;
}

void UTileBuffer::Clear()
{
    TileBuf.Empty();
}
