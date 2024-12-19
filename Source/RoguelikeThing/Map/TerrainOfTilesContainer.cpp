// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Map/TerrainOfTilesContainer.h"

DEFINE_LOG_CATEGORY(TerrainOfTilesContainer);

//Функция, восстанавливающая состояние всех переменных предзагрузки на основе массива ReCreationContainer при загрузке сохранения
void UTerrainOfTilesContainer::ReCreateTerrains(FMapDimensions MapDimensions, FCellCoord& MinNoEmptyTileCoord, FCellCoord& MaxNoEmptyTileCoord)
{
    //Сначала очищается матрица переменных предзагрузки, если там что-то было
    Clear();

    int TableLength = MapDimensions.TableLength;
    int MapTileLength = MapDimensions.MapTileLength;

    if (MapDimensions.isValid) {
        for (int row = MapDimensions.MinRow * TableLength; row < (MapDimensions.MaxRow + 1) * TableLength; row++) {
            int CurrentTileRow = (int)(row / MapTileLength);

            //Локальная координата ячейки равна остатку от деления глобальной координаты на длинну стороны тайла
            int CellRow = row % MapTileLength;

            //Если целевой строки нет, она создаётся
            if (!TerrainOfTilesRows.Contains(CurrentTileRow)) {
                UPROPERTY(SaveGame)
                UTerrainOfTilesRow* Row = NewObject<UTerrainOfTilesRow>();
                TerrainOfTilesRows.Add(CurrentTileRow, Row);
            }

            UPROPERTY(SaveGame)
            UTerrainOfTilesRow* TerrainOfTilesRow;
            if(TerrainOfTilesRows.Contains(CurrentTileRow)){
                TerrainOfTilesRow = *TerrainOfTilesRows.Find(CurrentTileRow);
            }
            else {
                UE_LOG(TerrainOfTilesContainer, Error, TEXT("!!! An error occurred in the TerrainOfTilesContainer class in the ReCreateTerrains function - Array TerrainOfTilesRows not contains valeu with key %d"), CurrentTileRow);
                break;
            }

            if (TerrainOfTilesRow) {
                for (int col = MapDimensions.MinCol * TableLength; col < (MapDimensions.MaxCol + 1) * TableLength; col++) {
                    int CurrentTileCol = (int)(col / MapTileLength);
                    bool HaveNoEmptyCells = false;

                    //Если целевой переменной предзагрузки нет, она создаётся
                    if (!TerrainOfTilesRow->TerrainOfTilesRow.Contains(CurrentTileCol)) {
                        UPROPERTY(SaveGame)
                        UTerrainOfTile* Terrain = NewObject<UTerrainOfTile>();
                        TerrainOfTilesRow->TerrainOfTilesRow.Add(CurrentTileCol, Terrain);
                    }

                    UPROPERTY(SaveGame)
                    UTerrainOfTile* Terrain;
                    if (TerrainOfTilesRow->TerrainOfTilesRow.Contains(CurrentTileCol)) {
                        Terrain = *TerrainOfTilesRow->TerrainOfTilesRow.Find(CurrentTileCol);
                    }
                    else {
                        UE_LOG(TerrainOfTilesContainer, Error, TEXT("!!! An error occurred in the TerrainOfTilesContainer class in the ReCreateTerrains function - Array TerrainOfTilesRow->TerrainOfTilesRow not contains valeu with key %d"), CurrentTileCol);
                        break;
                    }

                    if (Terrain) {
                        //Локальная координата ячейки равна остатку от деления глобальной координаты на длинну стороны тайла
                        int CellCol = col % MapTileLength;

                        FCellType CellType;
                        if (ReCreationContainer.Contains(FVector2D(row, col))) {
                            CellType = *ReCreationContainer.Find(FVector2D(row, col));
                        }
                        else {
                            UE_LOG(TerrainOfTilesContainer, Error, TEXT("!!! An error occurred in the TerrainOfTilesContainer class in the ReCreateTerrains function - Array ReCreationContainer not contains valeu with key (Row: %d ; Col: %d)"), row, col);
                            CellType = FCellType::Error;
                        }

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
                            UE_LOG(TerrainOfTilesContainer, Error, TEXT("!!! An error occurred in the TerrainOfTilesContainer class in the ReCreateTerrains function - CellType is of type Error"));
                        }
                    }
                    else {
                        UE_LOG(TerrainOfTilesContainer, Error, TEXT("!!! An error occurred in the TerrainOfTilesContainer class in the ReCreateTerrains function - Terrain is not valid"));
                    }
                }
            }
            else {
                UE_LOG(TerrainOfTilesContainer, Error, TEXT("!!! An error occurred in the TerrainOfTilesContainer class in the ReCreateTerrains function - TerrainOfTilesRow is not valid"));
            }
        }
    }
    else {
        UE_LOG(TerrainOfTilesContainer, Error, TEXT("!!! An error occurred in the TerrainOfTilesContainer class in the ReCreateTerrains function - MapDimensions is not valid"));
    }
}

//Функция полной очистки матрицы переменных предзагрузки
void UTerrainOfTilesContainer::Clear()
{
    if (TerrainOfTilesRows.Num() != 0) {
        TArray<UTerrainOfTilesRow*> Rows;
        TerrainOfTilesRows.GenerateValueArray(Rows);

        for (UTerrainOfTilesRow* Row : Rows) {
            if (Row && Row->TerrainOfTilesRow.Num() != 0) {
                TArray<UTerrainOfTile*> Terrains;
                Row->TerrainOfTilesRow.GenerateValueArray(Terrains);

                for (UTerrainOfTile* Terrain : Terrains) {
                    if (Terrain && Terrain->IsValidLowLevel()) {
                        Terrain->MarkPendingKill();
                    }
                }

                Row->TerrainOfTilesRow.Empty();
            }

            if (Row && Row->IsValidLowLevel()) {
                Row->MarkPendingKill();
            }
        }
        TerrainOfTilesRows.Empty();
    }
}
