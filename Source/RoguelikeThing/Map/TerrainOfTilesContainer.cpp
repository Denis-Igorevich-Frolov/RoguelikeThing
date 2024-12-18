// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Map/TerrainOfTilesContainer.h"

void UTerrainOfTilesContainer::ReCreateTerrains(FMapDimensions MapDimensions, FCellCoord& MinNoEmptyTileCoord, FCellCoord& MaxNoEmptyTileCoord)
{
    TerrainOfTilesRows.Empty();

    int TableLength = MapDimensions.TableLength;
    int MapTileLength = MapDimensions.MapTileLength;

    if (MapDimensions.isValid) {
        for (int row = MapDimensions.MinRow * TableLength; row < (MapDimensions.MaxRow + 1) * TableLength; row++) {
            int CurrentTileRow = (int)(row / MapTileLength);

            //Локальная координата ячейки равна остатку от деления глобальной координаты на длинну стороны тайла
            int CellRow = row % MapTileLength;

            //Если целевого столбца нет, он создаётся
            if (!TerrainOfTilesRows.Contains(CurrentTileRow)) {
                UPROPERTY(SaveGame)
                UTerrainOfTilesRow* Row = NewObject<UTerrainOfTilesRow>();
                TerrainOfTilesRows.Add(CurrentTileRow, Row);
            }

            UPROPERTY(SaveGame)
            UTerrainOfTilesRow* TerrainOfTilesCols = *TerrainOfTilesRows.Find(CurrentTileRow);

            if (TerrainOfTilesCols) {
                for (int col = MapDimensions.MinCol * TableLength; col < (MapDimensions.MaxCol + 1) * TableLength; col++) {
                    int CurrentTileCol = (int)(col / MapTileLength);
                    bool HaveNoEmptyCells = false;

                    //Если целевой строки нет, она создаётся
                    if (!TerrainOfTilesCols->TerrainOfTilesRow.Contains(CurrentTileCol)) {
                        UPROPERTY(SaveGame)
                        UTerrainOfTile* Terrain = NewObject<UTerrainOfTile>();
                        TerrainOfTilesCols->TerrainOfTilesRow.Add(CurrentTileCol, Terrain);
                    }

                    UPROPERTY(SaveGame)
                    UTerrainOfTile* Terrain = *TerrainOfTilesCols->TerrainOfTilesRow.Find(CurrentTileCol);

                    if (Terrain) {
                        //Локальная координата ячейки равна остатку от деления глобальной координаты на длинну стороны тайла
                        int CellCol = col % MapTileLength;

                        FCellType CellType = *ReCreationContainer.Find(FVector2D(row, col));

                        if (CellType == FCellType::Corridor || CellType == FCellType::Room) {
                            Terrain->AddCellType(FCellCoord(CellRow, CellCol), CellType);
                            //Проверка, не является ли текущий тайл минимальным или максимальным непустым тайлом
                            /* Если минимальные или максимальные координаты не проинициализированы ни одним
                             * другим тайлом, то координаты текущего тайла автоматически записываются */
                            if (MinNoEmptyTileCoord == FCellCoord(-1, -1)) {
                                MinNoEmptyTileCoord = FCellCoord(CurrentTileRow, CurrentTileCol);
                            }
                            if (MaxNoEmptyTileCoord == FCellCoord(-1, -1)) {
                                MaxNoEmptyTileCoord = FCellCoord(CurrentTileRow, CurrentTileCol);
                            }

                            if (CurrentTileRow < MinNoEmptyTileCoord.Row) {
                                MinNoEmptyTileCoord = FCellCoord(CurrentTileRow, MinNoEmptyTileCoord.Col);
                            }
                            else if (CurrentTileRow > MaxNoEmptyTileCoord.Row) {
                                MaxNoEmptyTileCoord = FCellCoord(CurrentTileRow, MaxNoEmptyTileCoord.Col);
                            }
                            if (CurrentTileCol < MinNoEmptyTileCoord.Col) {
                                MinNoEmptyTileCoord = FCellCoord(MinNoEmptyTileCoord.Row, CurrentTileCol);
                            }
                            else if (CurrentTileCol > MaxNoEmptyTileCoord.Col) {
                                MaxNoEmptyTileCoord = FCellCoord(MaxNoEmptyTileCoord.Row, CurrentTileCol);
                            }
                        }
                        else if (CellType == FCellType::Error) {
                            //UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the FillTerrainOfTiles function - CellType is of type Error"));
                        }
                    }
                    else {
                        //UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the FillTerrainOfTiles function - Terrain is not valid"));
                    }
                }
            }
            else {
                //UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the FillTerrainOfTiles function - TerrainOfTilesCols is not valid"));
            }
        }
    }
    else {
        //UE_LOG(MapMatrix, Error, TEXT("!!! An error occurred in the MapMatrix class in the FillTerrainOfTiles function - MapDimensions is not valid"));
    }
}
