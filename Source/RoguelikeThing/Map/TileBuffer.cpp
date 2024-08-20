// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Map/TileBuffer.h"
#include <RoguelikeThing/Widgets/MapEditor/MapTile.h>

DEFINE_LOG_CATEGORY(TileBuffer);

UAbstractTile* UTileBuffer::GetTile()
{
    if (!IsInit) {
        UE_LOG(LogTemp, Error, TEXT("aaa1"));
        return false;
    }

    if (TileBuf.Num() == 0) {
        UE_LOG(LogTemp, Error, TEXT("aaa1"));
        return nullptr;
    }

    UPROPERTY()
    UAbstractTile* Tile = TileBuf.Last();

    if (TileBuf.IsValidIndex(TileBuf.Num() - 1)) {
        TileBuf.RemoveAt(TileBuf.Num() - 1);
    }

    if (TileBuf.Num() < 10) {
        if(MapTileClass)
            if (!FillToMaximum()) {
                UE_LOG(LogTemp, Error, TEXT("AHA"));
            }
    }

    return Tile;
}

void UTileBuffer::AddTile(UAbstractTile* Tile)
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
}

bool UTileBuffer::FillToMaximum()
{
    if (!IsInit)
        return false;

    if (!TilesGridPanel)
        return false;

    int dif = MaxSize - TileBuf.Num();

    for (int i = 0; i < dif; i++) {
        if (TileBuf.Num() == MaxSize) {
            return true;
        }

        UPROPERTY()
        UMapTile* NewTile = CreateWidget<UMapTile>(TilesGridPanel, MapTileClass);
        if (!NewTile->FillingWithCells(MapTileLength, CellClass, MapEditor))
            return false;

        TileBuf.Insert(NewTile, 0);
    }

    return true;
}

void UTileBuffer::Clear()
{
    for (UAbstractTile* Tile : TileBuf) {
        if (Tile) {
            Tile->RemoveAllCells();
            Tile->ConditionalBeginDestroy();
            Tile->MarkPendingKill();
            Tile->RemoveFromParent();
        }
    }

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
