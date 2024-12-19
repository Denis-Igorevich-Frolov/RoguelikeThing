// Denis Igorevich Frolov did all this. Once there. All things reserved.

#include "RoguelikeThing/Map/TerrainOfTile.h"

DEFINE_LOG_CATEGORY(TerrainOfTile);

//Функция, добавляющая в переменную предзагрузки тайла тип не пустой ячейки
void UTerrainOfTile::AddCellType(FCellCoord Coord, FCellType CellType)
{
    //Если соответствующая строка уже есть в матрице, в неё просто добавляется переданное значение
    if (TerrainOfTileRows.Contains(Coord.Row)) {
        TMap<int, FCellType>* TerrainOfTileCols = TerrainOfTileRows.Find(Coord.Row);
        if (TerrainOfTileCols) {
            //Если по целевой координате уже есть какое-либо значение, его сначала следует удалить
            if (TerrainOfTileCols->Contains(Coord.Col)) {
                TerrainOfTileCols->Remove(Coord.Col);
            }

            TerrainOfTileCols->Add(Coord.Col, CellType);
        }
        else {
            UE_LOG(TerrainOfTile, Error, TEXT("!!! An error occurred in the TerrainOfTile class in the AddCellType function - TerrainOfTileCols is not valid"));
        }
    }
    //Иначе сначала создаётся строка, и в новую строку добавляется переданное значение
    else {
        TMap<int, FCellType> cols;
        cols.Add(Coord.Col, CellType);
        TerrainOfTileRows.Add(Coord.Row, cols);
    }
}

//Функция, возвращающая тип ячейки по переданной координате. Если стиль не обнаружен, возвращается FCellType::Error
FCellType UTerrainOfTile::GetCellType(FCellCoord Coord)
{
    if (TerrainOfTileRows.Contains(Coord.Row)) {
        TMap<int, FCellType>* TerrainOfTileCols = TerrainOfTileRows.Find(Coord.Row);
        if (TerrainOfTileCols) {
            if (TerrainOfTileCols->Contains(Coord.Col)) {
                return *TerrainOfTileCols->Find(Coord.Col);
            }
        }
        else {
            UE_LOG(TerrainOfTile, Error, TEXT("!!! An error occurred in the TerrainOfTile class in the GetCellType function - TerrainOfTileCols is not valid"));
        }
    }

    return FCellType::Error;
}

//Функция, проверяющая есть ли в переменной предзагрузки стиль по данной координате
bool UTerrainOfTile::Contains(FCellCoord Coord)
{
    if (TerrainOfTileRows.Contains(Coord.Row)) {
        TMap<int, FCellType>* TerrainOfTileCols = TerrainOfTileRows.Find(Coord.Row);
        if (TerrainOfTileCols) {
            if (TerrainOfTileCols->Contains(Coord.Col)) {
                return true;
            }
            else
                return false;
        }
        else {
            UE_LOG(TerrainOfTile, Error, TEXT("!!! An error occurred in the TerrainOfTile class in the Contains function - TerrainOfTileCols is not valid"));
            return false;
        }
    }
    else
        return false;
}

//Получение массива всех не нулевых ячеек тайла
TArray<FCellCoord> UTerrainOfTile::GetFilledCoord()
{
    TArray<int> TileRowsKeys;
    TerrainOfTileRows.GenerateKeyArray(TileRowsKeys);

    if (TileRowsKeys.Num() != 0) {
        TArray<FCellCoord> FilledCoord;

        for (int row : TileRowsKeys) {
            TMap<int, FCellType>* TerrainOfTileCols = TerrainOfTileRows.Find(row);

            if (TerrainOfTileCols) {
                TArray<int> TileColsKeys;

                TerrainOfTileCols->GenerateKeyArray(TileColsKeys);
                for (int col : TileColsKeys) {
                    FilledCoord.Add(FCellCoord(row, col));
                }
            }
            else {
                UE_LOG(TerrainOfTile, Error, TEXT("!!! An error occurred in the TerrainOfTile class in the GetFilledCoord function - TerrainOfTileCols is not valid"));
            }
        }

        return FilledCoord;
    }
    else {
        return TArray<FCellCoord>();
    }
}

//Функция, удаляющая тип ячейки по переданной координате из переменной предзагрузки. Функция возвращает удачно ли была выполнена операция
bool UTerrainOfTile::RemoveCell(FCellCoord Coord)
{
    if (TerrainOfTileRows.Contains(Coord.Row)) {
        TMap<int, FCellType>* TerrainOfTileCols = TerrainOfTileRows.Find(Coord.Row);
        if (TerrainOfTileCols) {
            if (TerrainOfTileCols->Contains(Coord.Col)) {
                TerrainOfTileCols->Remove(Coord.Col);

                //Если в строке больше нет ни одного элемента, то она удалется
                if (TerrainOfTileCols->Num() == 0) {
                    TerrainOfTileRows.Remove(Coord.Row);
                }

                return true;
            }
            else
                return false;
        }
        else {
            UE_LOG(TerrainOfTile, Error, TEXT("!!! An error occurred in the TerrainOfTile class in the RemoveCell function - TerrainOfTileCols is not valid"));
            return false;
        }
    }
    else
        return false;
}