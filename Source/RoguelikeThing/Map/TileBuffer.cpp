// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Map/TileBuffer.h"
#include <RoguelikeThing/Widgets/MapEditor/MapTile.h>
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(TileBuffer);

//Получение тайла из буфера, при этом в буфере переданный тайл удаляется
UAbstractTile* UTileBuffer::GetTile()
{
    if (!IsInit) {
        UE_LOG(TileBuffer, Error, TEXT("!!! An error occurred in the TileBuffer class in the GetTile function: The buffer was not initialized"));
        return false;
    }

    if (TileBuf.Num() == 0) {
        UE_LOG(TileBuffer, Error, TEXT("!!! An error occurred in the TileBuffer class in the GetTile function: The buffer was empty"));
        return nullptr;
    }

    UPROPERTY()
    UAbstractTile* Tile = TileBuf.Last();

    //Из буфера удаляется последний индекс - это индекс тайла, который сейчас будет передан
    if (TileBuf.IsValidIndex(TileBuf.Num() - 1)) {
        TileBuf.RemoveAt(TileBuf.Num() - 1);
    }
    else {
        UE_LOG(TileBuffer, Error, TEXT("!!! An error occurred in the TileBuffer class in the GetTile function: An error occurred while trying to delete the last index of the buffer - the index of the last tile is invalid"));
    }

    if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
        UE_LOG(TileBuffer, Log, TEXT("TileBuffer class in the GetTile function: Tile number %d is passed from the buffer"), TileBuf.Num() + 1);

    //Если в буфере стало меньше 10 талов, то он снова заполняется до максимума
    if (TileBuf.Num() < 10) {
        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileBuffer, Log, TEXT("TileBuffer class in the GetTile function: After the last tile transfer, the buffer contains less than 10 elements, the buffer will be filled to the maximum"));

        if (MapTileClass) {
            if (!FillToMaximum()) {
                UE_LOG(TileBuffer, Error, TEXT("!!! An error occurred in the TileBuffer class in the GetTile function: An error occurred while trying to fill the buffer to its maximum"));
            }
        }
        else {
            UE_LOG(TileBuffer, Error, TEXT("!!! An error occurred in the TileBuffer class in the GetTile function: MapTileClass is not valid"));
        }
    }

    return Tile;
}

//Добавляет переданный тайл в буфер
void UTileBuffer::AddTile(UAbstractTile* Tile)
{
    TileBuf.Add(Tile);
}

UTileBuffer::UTileBuffer()
{
    //Получение GameInstance из мира
    GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (!GameInstance)
        UE_LOG(TileBuffer, Warning, TEXT("Warning in FillingMapWithCells class in constructor - GameInstance was not retrieved from the world"));
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

//Заполняет буфер новыми тайлами до предела
bool UTileBuffer::FillToMaximum()
{
    if (!IsInit) {
        UE_LOG(TileBuffer, Error, TEXT("!!! An error occurred in the TileBuffer class in the FillToMaximum function: The buffer was not initialized"));
        return false;
    }

    if (!TilesGridPanel) {
        UE_LOG(TileBuffer, Error, TEXT("!!! An error occurred in the TileBuffer class in the FillToMaximum function: TilesGridPanel is not valid"));
        return false;
    }

    //Разница между максимальным количеством элементов в буфере и текущим
    int dif = MaxSize - TileBuf.Num();

    for (int i = 0; i < dif; i++) {
        //Если количество элементов в буфере сровнялось с максимумом, то заполнение прекращается
        if (TileBuf.Num() == MaxSize) {
            return true;
        }

        UPROPERTY()
        UMapTile* NewTile = CreateWidget<UMapTile>(TilesGridPanel, MapTileClass);
        if (!NewTile->FillingWithCells(MapTileLength, CellClass, MapEditor)) {
            UE_LOG(TileBuffer, Error, TEXT("!!! An error occurred in the TileBuffer class in the FillToMaximum function: An error occurred while trying to fill the tile with cells"));
            return false;
        }

        TileBuf.Insert(NewTile, 0);
    }

    return true;
}

//Полностью очищает буфер
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

//Функция, возвращающая текущее количество элементов в буфере
int UTileBuffer::BufSize()
{
    return TileBuf.Num();
}

//Функция, возвращающая максимальное количество элементов в буфере
int UTileBuffer::GetMaxSize()
{
    return MaxSize;
}
