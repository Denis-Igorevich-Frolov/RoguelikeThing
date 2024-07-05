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

    if (TileBuf.Num() < 10) {
        if(MapTileClass)
            ScoreToMaximum();
    }

    return Tile;
}

void UTileBuffer::AddTile(UUserWidget* Tile)
{
    TileBuf.Add(Tile);
}

void UTileBuffer::Init(int mapTileLength, UUniformGridPanel* refTilesGridPanel, UClass* refMapTileClass, UClass* refCellClass, UMapEditor* refMapEditor)
{
    this->MapTileLength = mapTileLength;
    TilesGridPanel = refTilesGridPanel;
    MapTileClass = refMapTileClass;
    CellClass = refCellClass;
    MapEditor = refMapEditor;

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
        if (TileBuf.Num() == MaxSize)
            return true;

        UMapTile* MapTile = StaticCast<UMapTile*>(CreateWidget<UUserWidget>(TilesGridPanel, MapTileClass));
        if (!MapTile->FillingWithCells(MapTileLength, CellClass, MapEditor))
            return false;

        TileBuf.Add(MapTile);
    }

    return true;
}

void UTileBuffer::Clear()
{
    TileBuf.Empty();
}

int UTileBuffer::BufSize()
{
    return TileBuf.Num();
}

int UTileBuffer::GetMaxSize()
{
    return MaxSize;
}
