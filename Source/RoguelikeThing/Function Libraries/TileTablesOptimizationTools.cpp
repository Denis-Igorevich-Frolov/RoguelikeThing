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

void UTileTablesOptimizationTools::AsynchronousAreaFilling(FGridDimensions AreaDimensions, int NumberOfMapTilesRows, FString ss)
{
    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [ss, AreaDimensions, NumberOfMapTilesRows, this]() {
        for (int col = AreaDimensions.Min.Col; col <= AreaDimensions.Max.Col; col++) {
            AsyncTask(ENamedThreads::GameThread, [ss, AreaDimensions, col, NumberOfMapTilesRows, this]() {
                for (int row = AreaDimensions.Min.Row; row <= AreaDimensions.Max.Row; row++) {
                    if (TilesCoordWrapper->FindWidget(row, col)) {
                        UE_LOG(TileTablesOptimizationTools, Warning, TEXT("%s add fffffff r: %d, c: %d"), *ss, row, col);
                        continue;
                    }
                    //else
                    //    UE_LOG(TileTablesOptimizationTools, Warning, TEXT("%s add ddddddd r: %d, c: %d"), *ss, row, col);

                    UUserWidget* Tile = TilesBuf->GetTile();

                    if (Tile) {
                        TilesCoordWrapper->AddWidget(row, col, Tile);
                        TilesGridPanel->AddChildToUniformGrid(Tile, (NumberOfMapTilesRows - row) - 1, col);
                    }
                    else
                        UE_LOG(TileTablesOptimizationTools, Warning, TEXT("AAAAAAAAAAAAA"));
                }
                });
        }
    });
}

void UTileTablesOptimizationTools::AsynchronousAreaRemoving(FGridDimensions AreaDimensions, int NumberOfMapTilesRows, FString ss)
{
    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [ss, AreaDimensions, NumberOfMapTilesRows, this]() {
        for (int col = AreaDimensions.Min.Col; col <= AreaDimensions.Max.Col; col++) {
            AsyncTask(ENamedThreads::GameThread, [ss, col, AreaDimensions, this]() {
                for (int row = AreaDimensions.Min.Row; row <= AreaDimensions.Max.Row; row++) {
                    if (!TilesCoordWrapper->RemoveWidget(row, col)) {
                        UE_LOG(TileTablesOptimizationTools, Warning, TEXT("%s remove fffffff r: %d, c: %d"), *ss, row, col);
                    }
                    //else
                        //UE_LOG(TileTablesOptimizationTools, Warning, TEXT("%s remove ddddddd r: %d, c: %d"), *ss, row, col);
                }
                });
        }
    });
}

void UTileTablesOptimizationTools::Init(UUniformGridPanel* refTilesGridPanel, UCoordWrapperOfTable* refTilesCoordWrapper, UTileBuffer* refTilesBuf, FGridDimensions originalDimensions)
{
    TilesGridPanel = refTilesGridPanel;
    TilesCoordWrapper = refTilesCoordWrapper;
    TilesBuf = refTilesBuf;

    this->OriginalDimensions = originalDimensions;
    OldDimensions = OriginalDimensions;
    CurrentDimensions = OriginalDimensions;

    IsInit = true;
}

void UTileTablesOptimizationTools::ChangeDisplayAreaFromShift(FVector2D TileShift, int NumberOfMapTilesRows)
{
    if (!IsInit) {
        return;
    }

    int TileShiftRow = trunc(TileShift.Y);
    int TileShiftCol = trunc(TileShift.X);

    CurrentDimensions = OriginalDimensions + FGridDimensions(FGridCoord(TileShiftRow, TileShiftCol), FGridCoord(TileShiftRow, TileShiftCol));

    FGridDimensions OffsetDifference = CurrentDimensions - OldDimensions;

    if (OldDimensions != CurrentDimensions) {
        //UE_LOG(TileTablesOptimizationTools, Warning, TEXT("old: %s, new %s"), *OldDimensions.ToString(), *CurrentDimensions.ToString());

        int MinRowCoordToFill = 0;
        int MaxRowCoordToFill = 0;
        int MinColCoordToFill = 0;
        int MaxColCoordToFill = 0;
        int MinRowCoordToRemove = 0;
        int MaxRowCoordToRemove = 0;
        int MinColCoordToRemove = 0;
        int MaxColCoordToRemove = 0;

        if (OffsetDifference.Max.Col > 0 && OffsetDifference.Max.Row == 0) {
            MinRowCoordToFill = CurrentDimensions.Min.Row;
            MaxRowCoordToFill = CurrentDimensions.Max.Row;

            MinRowCoordToRemove = CurrentDimensions.Min.Row;
            MaxRowCoordToRemove = CurrentDimensions.Max.Row;

            MinColCoordToFill = OldDimensions.Max.Col + 1;
            MaxColCoordToFill = CurrentDimensions.Max.Col;

            MinColCoordToRemove = OldDimensions.Min.Col;
            MaxColCoordToRemove = CurrentDimensions.Min.Col - 1;

            AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "pravo");
            AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "pravo");
        }
        else if (OffsetDifference.Max.Col < 0 && OffsetDifference.Max.Row == 0) {
            MinRowCoordToFill = CurrentDimensions.Min.Row;
            MaxRowCoordToFill = CurrentDimensions.Max.Row;

            MinRowCoordToRemove = CurrentDimensions.Min.Row;
            MaxRowCoordToRemove = CurrentDimensions.Max.Row;

            MinColCoordToFill = CurrentDimensions.Min.Col;
            MaxColCoordToFill = OldDimensions.Min.Col - 1;

            MinColCoordToRemove = CurrentDimensions.Max.Col + 1;
            MaxColCoordToRemove = OldDimensions.Max.Col;

            AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "levo");
            AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "levo");
        }
        else if (OffsetDifference.Max.Row > 0 && OffsetDifference.Max.Col == 0) {
            MinRowCoordToFill = OldDimensions.Max.Row + 1;
            MaxRowCoordToFill = CurrentDimensions.Max.Row;

            MinRowCoordToRemove = OldDimensions.Min.Row;
            MaxRowCoordToRemove = CurrentDimensions.Min.Row - 1;

            MinColCoordToFill = CurrentDimensions.Min.Col;
            MaxColCoordToFill = CurrentDimensions.Max.Col;

            MinColCoordToRemove = CurrentDimensions.Min.Col;
            MaxColCoordToRemove = CurrentDimensions.Max.Col;

            AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "verh");
            AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "verh");
        }
        else if (OffsetDifference.Max.Row < 0 && OffsetDifference.Max.Col == 0) {
            MinRowCoordToFill = CurrentDimensions.Min.Row;
            MaxRowCoordToFill = OldDimensions.Min.Row - 1;

            MinRowCoordToRemove = CurrentDimensions.Max.Row + 1;
            MaxRowCoordToRemove = OldDimensions.Max.Row;

            MinColCoordToFill = CurrentDimensions.Min.Col;
            MaxColCoordToFill = CurrentDimensions.Max.Col;

            MinColCoordToRemove = CurrentDimensions.Min.Col;
            MaxColCoordToRemove = CurrentDimensions.Max.Col;

            AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "niz");
            AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "niz");
        }
        else if (OffsetDifference.Max.Col > 0 && OffsetDifference.Max.Row > 0) {
            MinRowCoordToFill = OldDimensions.Max.Row + 1;
            MaxRowCoordToFill = CurrentDimensions.Max.Row;

            MinRowCoordToRemove = OldDimensions.Min.Row;
            MaxRowCoordToRemove = CurrentDimensions.Min.Row - 1;

            MinColCoordToFill = CurrentDimensions.Min.Col;
            MaxColCoordToFill = CurrentDimensions.Max.Col;

            MinColCoordToRemove = OldDimensions.Min.Col;
            MaxColCoordToRemove = OldDimensions.Max.Col;

            AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "pravo verh");
            AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "pravo verh");

            MinRowCoordToFill = CurrentDimensions.Min.Row;
            MaxRowCoordToFill = OldDimensions.Max.Row;

            MinRowCoordToRemove = CurrentDimensions.Min.Row;
            MaxRowCoordToRemove = OldDimensions.Max.Row;

            MinColCoordToFill = OldDimensions.Max.Col + 1;
            MaxColCoordToFill = CurrentDimensions.Max.Col;

            MinColCoordToRemove = OldDimensions.Min.Col;
            MaxColCoordToRemove = CurrentDimensions.Min.Col - 1;

            AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "pravo verh");
            AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "pravo verh");
        }
        else if (OffsetDifference.Max.Col > 0 && OffsetDifference.Max.Row < 0) {
            MinRowCoordToFill = CurrentDimensions.Min.Row;
            MaxRowCoordToFill = CurrentDimensions.Max.Row;

            MinRowCoordToRemove = OldDimensions.Min.Row;
            MaxRowCoordToRemove = OldDimensions.Max.Row;

            MinColCoordToFill = OldDimensions.Max.Col + 1;
            MaxColCoordToFill = CurrentDimensions.Max.Col;

            MinColCoordToRemove = OldDimensions.Min.Col;
            MaxColCoordToRemove = CurrentDimensions.Min.Col - 1;

            AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "pravo niz");
            AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "pravo niz");

            MinRowCoordToFill = CurrentDimensions.Min.Row;
            MaxRowCoordToFill = OldDimensions.Min.Row - 1;

            MinRowCoordToRemove = CurrentDimensions.Max.Row + 1;
            MaxRowCoordToRemove = OldDimensions.Max.Row;

            MinColCoordToFill = CurrentDimensions.Min.Col;
            MaxColCoordToFill = OldDimensions.Max.Col;

            MinColCoordToRemove = CurrentDimensions.Min.Col;
            MaxColCoordToRemove = OldDimensions.Max.Col;

            AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "pravo niz");
            AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "pravo niz");
        }
        else if (OffsetDifference.Max.Col < 0 && OffsetDifference.Max.Row > 0) {
            MinRowCoordToFill = OldDimensions.Max.Row + 1;
            MaxRowCoordToFill = CurrentDimensions.Max.Row;

            MinRowCoordToRemove = OldDimensions.Min.Row;
            MaxRowCoordToRemove = CurrentDimensions.Min.Row - 1;

            MinColCoordToFill = CurrentDimensions.Min.Col;
            MaxColCoordToFill = CurrentDimensions.Max.Col;

            MinColCoordToRemove = OldDimensions.Min.Col;
            MaxColCoordToRemove = OldDimensions.Max.Col;

            AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "levo verh");
            AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "levo verh");

            MinRowCoordToFill = CurrentDimensions.Min.Row;
            MaxRowCoordToFill = OldDimensions.Max.Row;

            MinRowCoordToRemove = CurrentDimensions.Min.Row;
            MaxRowCoordToRemove = OldDimensions.Max.Row;

            MinColCoordToFill = CurrentDimensions.Min.Col;
            MaxColCoordToFill = OldDimensions.Min.Col - 1;

            MinColCoordToRemove = CurrentDimensions.Max.Col + 1;
            MaxColCoordToRemove = OldDimensions.Max.Col;

            AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "levo verh");
            AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "levo verh");
        }
        else if (OffsetDifference.Max.Col < 0 && OffsetDifference.Max.Row < 0) {
            MinRowCoordToFill = CurrentDimensions.Min.Row;
            MaxRowCoordToFill = OldDimensions.Min.Row - 1;

            MinRowCoordToRemove = CurrentDimensions.Max.Row + 1;
            MaxRowCoordToRemove = OldDimensions.Max.Row;

            MinColCoordToFill = CurrentDimensions.Min.Col;
            MaxColCoordToFill = CurrentDimensions.Max.Col;

            MinColCoordToRemove = OldDimensions.Min.Col;
            MaxColCoordToRemove = OldDimensions.Max.Col;

            AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "levo niz");
            AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "levo niz");

            MinRowCoordToFill = OldDimensions.Min.Row;
            MaxRowCoordToFill = CurrentDimensions.Max.Row;

            MinRowCoordToRemove = OldDimensions.Min.Row;
            MaxRowCoordToRemove = CurrentDimensions.Max.Row;

            MinColCoordToFill = CurrentDimensions.Min.Col;
            MaxColCoordToFill = OldDimensions.Min.Col - 1;

            MinColCoordToRemove = CurrentDimensions.Max.Col + 1;
            MaxColCoordToRemove = OldDimensions.Max.Col;

            AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "levo niz");
            AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "levo niz");
        }

        TilesCoordWrapper->setMinCoord(CurrentDimensions.Min);
        TilesCoordWrapper->setMaxCoord(CurrentDimensions.Max);
    }

    OldDimensions = CurrentDimensions;
}
