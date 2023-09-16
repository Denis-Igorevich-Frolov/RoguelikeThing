// Fill out your copyright notice in the Description page of Project Settings.


#include "RoguelikeThing/Function Libraries/TileTablesOptimizationTools.h"

void UTileTablesOptimizationTools::InitializingUncollapseOfTiles(UUniformGridPanel* TileGridPanel,
    FVector2D MinimumTileSize, FVector2D WidgetAreaSize, FMapDimensions MapDimensions)
{
    int TileLen = MapDimensions.MapTileLength;
    int TableRows = (MapDimensions.MaxRow - MapDimensions.MinRow + 1) * (MapDimensions.TableLength / TileLen);
    int TableCols = (MapDimensions.MaxCol - MapDimensions.MinCol + 1) * (MapDimensions.TableLength / TileLen);

    if (TableRows > 3 * (MapDimensions.TableLength / TileLen))
        TableRows = 3 * (MapDimensions.TableLength / TileLen);
    if (TableCols > 3 * (MapDimensions.TableLength / TileLen))
        TableCols = 3 * (MapDimensions.TableLength / TileLen);

    FVector2D MinimumTableSize = MinimumTileSize * FVector2D(TableRows, TableCols);

    UE_LOG(LogTemp, Error, TEXT("WidgetAreaSize: %s"), *WidgetAreaSize.ToString());
    UE_LOG(LogTemp, Error, TEXT("MinimumTileSize: %s"), *MinimumTileSize.ToString());
    UE_LOG(LogTemp, Error, TEXT("MinimumTableSize: %s"), *MinimumTableSize.ToString());

    FVector2D SizeDifference = MinimumTableSize - WidgetAreaSize;

    UE_LOG(LogTemp, Error, TEXT("SizeDifference: %s"), *SizeDifference.ToString());

    int NumberOfCollapsedTilesTopAndBottom = (SizeDifference.Y / 2) / MinimumTileSize.Y;
    int NumberOfCollapsedTilesRinhtAndLeft = (SizeDifference.X / 2) / MinimumTileSize.X;

    UE_LOG(LogTemp, Error, TEXT("%d, %d"), NumberOfCollapsedTilesTopAndBottom, NumberOfCollapsedTilesRinhtAndLeft);

    int NumberOfItemsInTable = TileGridPanel->GetAllChildren().Num();
    UE_LOG(LogTemp, Error, TEXT("NumberOfItemsInTable: %d, NumberOfRows: %d, NumberOfCols: %d"), NumberOfItemsInTable, TableRows, TableCols);

    if (TileGridPanel->HasAnyChildren()) {
        for (int row = NumberOfCollapsedTilesRinhtAndLeft; row < TableRows - NumberOfCollapsedTilesRinhtAndLeft; row++) {
            for (int col = NumberOfCollapsedTilesTopAndBottom; col < TableCols - NumberOfCollapsedTilesTopAndBottom; col++) {
                UE_LOG(LogTemp, Error, TEXT("cell: %d, row: %d, col: %d"), row * TableCols + col, row, col);
                UWidget* GridPanelElement = TileGridPanel->GetChildAt(row * TableCols + col);
                if (GridPanelElement)
                    GridPanelElement->SetVisibility(ESlateVisibility::Visible);
                else
                    UE_LOG(LogTemp, Error, TEXT("CHORT"));
            }
        }

        UWidget* HiddenWidget = TileGridPanel->GetChildAt(0);
        if(HiddenWidget && HiddenWidget->GetVisibility() == ESlateVisibility::Collapsed)
            HiddenWidget->SetVisibility(ESlateVisibility::Hidden);

        HiddenWidget = TileGridPanel->GetChildAt(NumberOfItemsInTable - 1);
        if (HiddenWidget && HiddenWidget->GetVisibility() == ESlateVisibility::Collapsed)
            HiddenWidget->SetVisibility(ESlateVisibility::Hidden);

    }
}
