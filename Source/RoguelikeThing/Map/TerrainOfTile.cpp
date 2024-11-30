// Denis Igorevich Frolov did all this. Once there. All things reserved.

#include "RoguelikeThing/Map/TerrainOfTile.h"

DEFINE_LOG_CATEGORY(TerrainOfTile);

void UTerrainOfTile::AddCellType(FCellCoord Coord, FCellType CellType)
{
    //���� ��������������� ������ ��� ���� � �������, � �� ������ ����������� ���������� ��������
    if (TerrainOfTileRows.TerrainOfTileRows.Contains(Coord.Row)) {
        TMap<int, FCellType>* TerrainOfTileCols = TerrainOfTileRows.TerrainOfTileRows.Find(Coord.Row);
        if (TerrainOfTileCols) {
            //���� �� ������� ���������� ��� ���� �����-���� ��������, ��� ������� ������� �������
            if (TerrainOfTileCols->Contains(Coord.Col)) {
                TerrainOfTileCols->Remove(Coord.Col);
            }

            TerrainOfTileCols->Add(Coord.Col, CellType);
        }
        else {
            UE_LOG(TerrainOfTile, Error, TEXT("!!! An error occurred in the TerrainOfTile class in the AddCellType function - TerrainOfTileCols is not valid"));
        }
    }
    //����� ������� �������� ������, � � ����� ������ ����������� ���������� ��������
    else {
        TMap<int, FCellType> cols;
        cols.Add(Coord.Col, CellType);
        TerrainOfTileRows.TerrainOfTileRows.Add(Coord.Row, cols);
    }
}

FCellType UTerrainOfTile::GetCellType(FCellCoord Coord)
{
    if (TerrainOfTileRows.TerrainOfTileRows.Contains(Coord.Row)) {
        TMap<int, FCellType>* TerrainOfTileCols = TerrainOfTileRows.TerrainOfTileRows.Find(Coord.Row);
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

bool UTerrainOfTile::Contains(FCellCoord Coord)
{
    if (TerrainOfTileRows.TerrainOfTileRows.Contains(Coord.Row)) {
        TMap<int, FCellType>* TerrainOfTileCols = TerrainOfTileRows.TerrainOfTileRows.Find(Coord.Row);
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

//��������� ������� ���� �� ������� ����� �����
TArray<FCellCoord> UTerrainOfTile::GetFilledCoord()
{
    TArray<int> TileRowsKeys;
    TerrainOfTileRows.TerrainOfTileRows.GenerateKeyArray(TileRowsKeys);

    if (TileRowsKeys.Num() != 0) {
        TArray<FCellCoord> FilledCoord;

        for (int row : TileRowsKeys) {
            TMap<int, FCellType>* TerrainOfTileCols = TerrainOfTileRows.TerrainOfTileRows.Find(row);

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

bool UTerrainOfTile::RemoveCell(FCellCoord Coord)
{
    if (TerrainOfTileRows.TerrainOfTileRows.Contains(Coord.Row)) {
        TMap<int, FCellType>* TerrainOfTileCols = TerrainOfTileRows.TerrainOfTileRows.Find(Coord.Row);
        if (TerrainOfTileCols) {
            if (TerrainOfTileCols->Contains(Coord.Col)) {
                TerrainOfTileCols->Remove(Coord.Col);

                //���� � ������ ������ ��� �� ������ ��������, �� ��� ��������
                if (TerrainOfTileCols->Num() == 0) {
                    TerrainOfTileRows.TerrainOfTileRows.Remove(Coord.Row);
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