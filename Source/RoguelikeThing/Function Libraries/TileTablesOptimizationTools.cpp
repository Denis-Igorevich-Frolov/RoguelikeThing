// Denis Igorevich Frolov did all this. Once there. All things reserved.

#include "RoguelikeThing/Function Libraries/TileTablesOptimizationTools.h"
#include <Kismet/GameplayStatics.h>

#pragma optimize("", off)

DEFINE_LOG_CATEGORY(TileTablesOptimizationTools);

UTileTablesOptimizationTools::UTileTablesOptimizationTools()
{
    //Получение GameInstance из мира
    GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (!GameInstance)
        UE_LOG(TileTablesOptimizationTools, Warning, TEXT("Warning in TileTablesOptimizationTools class in constructor - GameInstance was not retrieved from the world"));
}

void UTileTablesOptimizationTools::Init(UUniformGridPanel* tilesGridPanel, UCoordWrapperOfTable* tilesCoordWrapper, UTileBuffer* tilesBuf, FGridDimensions originalDimensions)
{
    this->TilesGridPanel = tilesGridPanel;
    this->TilesCoordWrapper = tilesCoordWrapper;
    this->TilesBuf = tilesBuf;

    this->OriginalDimensions = originalDimensions;
    OldDimensions = OriginalDimensions;
    CurrentDimensions = OriginalDimensions;

    IsInit = true;
}

void UTileTablesOptimizationTools::ChangeDisplayAreaFromShift(FVector2D TileShift)
{
    if (!IsInit) {
        return;
    }

    int TileShiftRow = trunc(TileShift.Y);
    int TileShiftCol = trunc(TileShift.X);

    CurrentDimensions = OriginalDimensions + FGridDimensions(FGridCoord(TileShiftRow, TileShiftCol), FGridCoord(TileShiftRow, TileShiftCol));
    FGridDimensions OffsetDifference = CurrentDimensions - OldDimensions;

    if (OldDimensions != CurrentDimensions) {
        if (OffsetDifference.Max.Col > 0) {
            for (int col = 0; col < OffsetDifference.Max.Col; col++) {
                UE_LOG(TileTablesOptimizationTools, Warning, TEXT("old: %s, new %s"), *OldDimensions.ToString(), *CurrentDimensions.ToString());
                for (int row = CurrentDimensions.Min.Row; row <= CurrentDimensions.Max.Row; row++) {
                    if (TilesCoordWrapper->FindWidget(row, OldDimensions.Max.Col + col + 1)) {
                        //UE_LOG(TileTablesOptimizationTools, Warning, TEXT("fffffff r: %d, c: %d"), row, OldDimensions.Max.Col + col + 1);
                        continue;
                    }
                    //else
                    //    UE_LOG(TileTablesOptimizationTools, Warning, TEXT("ddddddd r: %d, c: %d"), row, OldDimensions.Max.Col + col + 1);

                    UUserWidget* Tile = TilesBuf->GetTile();

                    if (!Tile) {
                        UE_LOG(TileTablesOptimizationTools, Warning, TEXT("AAAAAAAAAAAAA"));
                    }

                    TilesCoordWrapper->AddWidget(row, OldDimensions.Max.Col + col + 1, Tile);

                    TilesGridPanel->AddChildToUniformGrid(Tile, row, OldDimensions.Max.Col + col + 1);
                }

                for (int row = OldDimensions.Min.Row; row <= OldDimensions.Max.Row; row++) {
                    if (!TilesCoordWrapper->RemoveWidget(row, OldDimensions.Min.Col + col)) {
                        //UE_LOG(TileTablesOptimizationTools, Warning, TEXT("fffffff r: %d, c: %d"), row, OldDimensions.Min.Col + col);
                    }
                    //else
                        //UE_LOG(TileTablesOptimizationTools, Warning, TEXT("ddddddd r: %d, c: %d"), row, OldDimensions.Min.Col + col);
                }
            }
        }

    }

    OldDimensions = CurrentDimensions;
}
