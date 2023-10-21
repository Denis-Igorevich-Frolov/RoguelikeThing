// Fill out your copyright notice in the Description page of Project Settings.


#include "RoguelikeThing/Function Libraries/TileTablesOptimizationTools.h"

FVector2D UTileTablesOptimizationTools::InitTableTiles(UUniformGridPanel* TileGridPanel,
    FVector2D TileSize, FVector2D WidgetAreaSize, FMapDimensions MapDimensions)
{
    if (MapDimensions.isValid) {
        if (TileGridPanel->HasAnyChildren()) {
            TileLen = MapDimensions.MapTileLength;
            int NumberOfTilesInFragment = MapDimensions.TableLength / TileLen;

            TableRows = (MapDimensions.MaxRow - MapDimensions.MinRow + 1) * NumberOfTilesInFragment;
            TableCols = (MapDimensions.MaxCol - MapDimensions.MinCol + 1) * NumberOfTilesInFragment;

            RealTableRows = TableRows;
            RealTableCols = TableCols;

            if (TableRows > 3 * NumberOfTilesInFragment)
                TableRows = 3 * NumberOfTilesInFragment;
            if (TableCols > 3 * NumberOfTilesInFragment)
                TableCols = 3 * NumberOfTilesInFragment;

            OriginalTileSize = TileSize;
            widgetAreaSize = WidgetAreaSize;

            OriginalTableSize = OriginalTileSize * FVector2D(TableCols, TableRows);
            SizeDifference = OriginalTableSize - widgetAreaSize;

            int NumberOfCollapsedTilesTopAndBottom = (SizeDifference.Y / 2) / OriginalTileSize.Y;
            int NumberOfCollapsedTilesRinhtAndLeft = (SizeDifference.X / 2) / OriginalTileSize.X;

            if (NumberOfCollapsedTilesTopAndBottom < 0)
                NumberOfCollapsedTilesTopAndBottom = 0;
            if (NumberOfCollapsedTilesRinhtAndLeft < 0)
                NumberOfCollapsedTilesRinhtAndLeft = 0;

            UE_LOG(LogTemp, Error, TEXT("%d, %d"), NumberOfCollapsedTilesTopAndBottom, NumberOfCollapsedTilesRinhtAndLeft);

            int NumberOfItemsInTable = TileGridPanel->GetAllChildren().Num();
            UE_LOG(LogTemp, Error, TEXT("NumberOfItemsInTable: %d, NumberOfRows: %d, NumberOfCols: %d"), NumberOfItemsInTable, TableRows, TableCols);


            CurrentDimensions = FDimensionsDisplayedArea(
                FTileCoord(NumberOfCollapsedTilesRinhtAndLeft, NumberOfCollapsedTilesTopAndBottom),
                FTileCoord(TableCols - NumberOfCollapsedTilesRinhtAndLeft - 1, TableRows - NumberOfCollapsedTilesTopAndBottom - 1));

            UE_LOG(LogTemp, Error, TEXT("CurrentDimensions: %s"), *CurrentDimensions.ToString());

            for (int row = CurrentDimensions.Min.Y; row <= CurrentDimensions.Max.Y; row++) {
                for (int col = CurrentDimensions.Min.X; col <= CurrentDimensions.Max.X; col++) {
                    UWidget* GridPanelElement = TileGridPanel->GetChildAt(row * TableCols + col);
                    if (GridPanelElement)
                        GridPanelElement->SetVisibility(ESlateVisibility::Visible);
                    else
                        UE_LOG(LogTemp, Error, TEXT("CHORT"));
                }
            }

            UWidget* HiddenWidget = TileGridPanel->GetChildAt(0);
            if (HiddenWidget && HiddenWidget->GetVisibility() == ESlateVisibility::Collapsed)
                HiddenWidget->SetVisibility(ESlateVisibility::Hidden);

            HiddenWidget = TileGridPanel->GetChildAt(NumberOfItemsInTable - 1);
            if (HiddenWidget && HiddenWidget->GetVisibility() == ESlateVisibility::Collapsed)
                HiddenWidget->SetVisibility(ESlateVisibility::Hidden);

            OriginalDimensions = CurrentDimensions;
            OldDimensions = CurrentDimensions;

            OriginalDimensionsSize = FVector2D((OriginalDimensions.Max.X - OriginalDimensions.Min.X) * OriginalTileSize.X,
                (OriginalDimensions.Max.Y - OriginalDimensions.Min.Y) * OriginalTileSize.Y);

            return OriginalTableSize;
        }
    }
    return FVector2D();
}

void UTileTablesOptimizationTools::ChangingVisibilityOfTableTiles(UUniformGridPanel* TileGridPanel, FVector2D Bias, float ZoomMultiplier)
{
    Bias.X *= -1;

    FTileCoord MinBiasCoord;
    FTileCoord MaxBiasCoord;


    if (Bias.X > 0) {
        if (Bias.X < SizeDifference.X / 2 - OriginalTileSize.X) {
            MinBiasCoord.X = floor(Bias.X / OriginalTileSize.X);
            MaxBiasCoord.X = ceil(Bias.X / OriginalTileSize.X);
        }
        else {
            MinBiasCoord.X = floor((SizeDifference.X / 2 - OriginalTileSize.X) / OriginalTileSize.X);
            MaxBiasCoord.X = ceil((SizeDifference.X / 2 - OriginalTileSize.X) / OriginalTileSize.X);
        }
    }
    else {
        if (Bias.X > -(SizeDifference.X / 2 - OriginalTileSize.X)) {
            MinBiasCoord.X = floor(Bias.X / OriginalTileSize.X);
            MaxBiasCoord.X = ceil(Bias.X / OriginalTileSize.X);
        }
        else {
            MinBiasCoord.X = floor(-(SizeDifference.X / 2 - OriginalTileSize.X) / OriginalTileSize.X);
            MaxBiasCoord.X = ceil(-(SizeDifference.X / 2 - OriginalTileSize.X) / OriginalTileSize.X);
        }
    }
    if (Bias.Y > 0) {
        if (Bias.Y < SizeDifference.Y / 2 - OriginalTileSize.Y) {
            MinBiasCoord.Y = floor(Bias.Y / OriginalTileSize.Y);
            MaxBiasCoord.Y = ceil(Bias.Y / OriginalTileSize.Y);
        }
        else {
            MinBiasCoord.Y = floor((SizeDifference.Y / 2 - OriginalTileSize.Y) / OriginalTileSize.Y);
            MaxBiasCoord.Y = ceil((SizeDifference.Y / 2 - OriginalTileSize.Y) / OriginalTileSize.Y);
        }
    }
    else {
        if (Bias.Y > -(SizeDifference.Y / 2 - OriginalTileSize.Y)) {
            MinBiasCoord.Y = floor(Bias.Y / OriginalTileSize.Y);
            MaxBiasCoord.Y = ceil(Bias.Y / OriginalTileSize.Y);
        }
        else {
            MinBiasCoord.Y = floor(-(SizeDifference.Y / 2 - OriginalTileSize.Y) / OriginalTileSize.Y);
            MaxBiasCoord.Y = ceil(-(SizeDifference.Y / 2 - OriginalTileSize.Y) / OriginalTileSize.Y);
        }
    }

    FDimensionsDisplayedArea BiansDimentions = FDimensionsDisplayedArea(MinBiasCoord, MaxBiasCoord);
    FDimensionsDisplayedArea ZoomDimentions = FDimensionsDisplayedArea();

    if (ZoomMultiplier > 0)
    {
        FTileCoord MinZoomCoord;
        FTileCoord MaxZoomCoord;

        MaxZoomCoord.X = ceil((OriginalDimensionsSize.X / ZoomMultiplier - widgetAreaSize.X) / OriginalTileSize.X) / 2;
        MinZoomCoord.X = -MaxZoomCoord.X;
        MaxZoomCoord.Y = ceil((OriginalDimensionsSize.Y / ZoomMultiplier - widgetAreaSize.Y) / OriginalTileSize.Y) / 2;
        MinZoomCoord.Y = -MaxZoomCoord.Y;

        ZoomDimentions = FDimensionsDisplayedArea(MinZoomCoord, MaxZoomCoord);
    }

    CurrentDimensions = OriginalDimensions + BiansDimentions + ZoomDimentions;

    int NumberOfItemsInTable = TileGridPanel->GetAllChildren().Num();

    if (TileGridPanel->HasAnyChildren()) {
        if (OldDimensions != CurrentDimensions){
            bool NewTopBoundMore = (CurrentDimensions.Max.Y > OldDimensions.Max.Y)/* &&
                (Bias.Y < (OriginalTableSize.Y - widgetAreaSize.Y) / 2 - OriginalTileSize.X / 2)*/;
            bool OldTopBoundLess = CurrentDimensions.Max.Y < OldDimensions.Max.Y/* &&
                (Bias.Y < (OriginalTableSize.Y - widgetAreaSize.Y) / 2 - OriginalTileSize.X / 2)*/;
            bool NewBottomBoundMore = CurrentDimensions.Min.Y < OldDimensions.Min.Y/* &&
                (Bias.Y > -((OriginalTableSize.Y - widgetAreaSize.Y) / 2 - OriginalTileSize.X / 2))*/;
            bool OldBottomBoundLess = CurrentDimensions.Min.Y > OldDimensions.Min.Y/* &&
                (Bias.Y > -((OriginalTableSize.Y - widgetAreaSize.Y) / 2 - OriginalTileSize.X / 2))*/;
            bool NewLeftBoundMore = CurrentDimensions.Min.X < OldDimensions.Min.X/* &&
                (Bias.X > -((OriginalTableSize.X - widgetAreaSize.X) / 2 - OriginalTileSize.X / 2))*/;
            bool OldLeftBoundLess = CurrentDimensions.Min.X > OldDimensions.Min.X/* &&
                (Bias.X > -((OriginalTableSize.X - widgetAreaSize.X) / 2 - OriginalTileSize.X / 2))*/;
            bool NewRightBoundMore = CurrentDimensions.Max.X > OldDimensions.Max.X/* &&
                (Bias.X < (OriginalTableSize.X - widgetAreaSize.X) / 2 - OriginalTileSize.X / 2)*/;
            bool OldRightBoundLess = CurrentDimensions.Max.X < OldDimensions.Max.X/* &&
                (Bias.X < (OriginalTableSize.X - widgetAreaSize.X) / 2 - OriginalTileSize.X / 2)*/;

            if (NewTopBoundMore) {
                for (int row = OldDimensions.Max.Y + 1; row <= CurrentDimensions.Max.Y; row++) {
                    for (int col = CurrentDimensions.Min.X; col <= CurrentDimensions.Max.X; col++) {
                        UWidget* GridPanelElement = TileGridPanel->GetChildAt(row * TableCols + col);
                        if (GridPanelElement) {
                            GridPanelElement->SetVisibility(ESlateVisibility::Visible);
                        }
                        else
                            UE_LOG(LogTemp, Error, TEXT("NewTopBoundMore CHORT"));
                    }
                }
            }

            if(OldTopBoundLess){
                for (int row = CurrentDimensions.Max.Y + 1; row <= OldDimensions.Max.Y; row++) {
                    for (int col = OldDimensions.Min.X; col <= OldDimensions.Max.X; col++) {
                        UWidget* GridPanelElement = TileGridPanel->GetChildAt(row * TableCols + col);
                        if (GridPanelElement) {
                            GridPanelElement->SetVisibility(ESlateVisibility::Collapsed);
                        }
                        else
                            UE_LOG(LogTemp, Error, TEXT("OldTopBoundLess CHORT"));
                    }
                }
            }

            if (NewBottomBoundMore) {
                for (int row = CurrentDimensions.Min.Y; row <= OldDimensions.Min.Y - 1; row++) {
                    for (int col = CurrentDimensions.Min.X; col <= CurrentDimensions.Max.X; col++) {
                        UWidget* GridPanelElement = TileGridPanel->GetChildAt(row * TableCols + col);
                        if (GridPanelElement) {
                            GridPanelElement->SetVisibility(ESlateVisibility::Visible);
                        }
                        else
                            UE_LOG(LogTemp, Error, TEXT("NewBottomBoundMore CHORT"));
                    }
                }
            }

            if (OldBottomBoundLess) {
                for (int row = OldDimensions.Min.Y; row <= CurrentDimensions.Min.Y - 1; row++) {
                    for (int col = OldDimensions.Min.X; col <= OldDimensions.Max.X; col++) {
                        UWidget* GridPanelElement = TileGridPanel->GetChildAt(row * TableCols + col);
                        if (GridPanelElement) {
                            GridPanelElement->SetVisibility(ESlateVisibility::Collapsed);
                        }
                        else
                            UE_LOG(LogTemp, Error, TEXT("OldBottomBoundLess CHORT"));
                    }
                }
            }

            if (NewLeftBoundMore) {
                for (int row = CurrentDimensions.Min.Y; row <= CurrentDimensions.Max.Y; row++) {
                    for (int col = CurrentDimensions.Min.X; col <= OldDimensions.Min.X - 1; col++) {
                        UWidget* GridPanelElement = TileGridPanel->GetChildAt(row * TableCols + col);
                        if (GridPanelElement) {
                            GridPanelElement->SetVisibility(ESlateVisibility::Visible);
                        }
                        else
                            UE_LOG(LogTemp, Error, TEXT("NewLeftBoundMore CHORT"));
                    }
                }
            }

            if (OldLeftBoundLess) {
                for (int row = OldDimensions.Min.Y; row <= OldDimensions.Max.Y; row++) {
                    for (int col = OldDimensions.Min.X; col <= CurrentDimensions.Min.X - 1; col++) {
                        UWidget* GridPanelElement = TileGridPanel->GetChildAt(row * TableCols + col);
                        if (GridPanelElement) {
                            GridPanelElement->SetVisibility(ESlateVisibility::Collapsed);
                        }
                        else
                            UE_LOG(LogTemp, Error, TEXT("OldLeftBoundLess CHORT"));
                    }
                }
            }

            if (NewRightBoundMore) {
                for (int row = CurrentDimensions.Min.Y; row <= CurrentDimensions.Max.Y; row++) {
                    for (int col = OldDimensions.Max.X + 1; col <= CurrentDimensions.Max.X; col++) {
                        UWidget* GridPanelElement = TileGridPanel->GetChildAt(row * TableCols + col);
                        if (GridPanelElement) {
                            GridPanelElement->SetVisibility(ESlateVisibility::Visible);
                        }
                        else
                            UE_LOG(LogTemp, Error, TEXT("NewRightBoundMore CHORT"));
                    }
                }
            }

            if (OldRightBoundLess) {
                for (int row = OldDimensions.Min.Y; row <= OldDimensions.Max.Y; row++) {
                    for (int col = CurrentDimensions.Max.X + 1; col <= OldDimensions.Max.X; col++) {
                        UWidget* GridPanelElement = TileGridPanel->GetChildAt(row * TableCols + col);
                        if (GridPanelElement) {
                            GridPanelElement->SetVisibility(ESlateVisibility::Collapsed);
                        }
                        else
                            UE_LOG(LogTemp, Error, TEXT("OldRightBoundLess CHORT"));
                    }
                }
            }
        }

        UWidget* HiddenWidget = TileGridPanel->GetChildAt(0);
        if(HiddenWidget && HiddenWidget->GetVisibility() == ESlateVisibility::Collapsed)
            HiddenWidget->SetVisibility(ESlateVisibility::Hidden);

        HiddenWidget = TileGridPanel->GetChildAt(NumberOfItemsInTable - 1);
        if (HiddenWidget && HiddenWidget->GetVisibility() == ESlateVisibility::Collapsed)
            HiddenWidget->SetVisibility(ESlateVisibility::Hidden);

    }

    OldDimensions = CurrentDimensions;
}

FTileCoord::FTileCoord(int32 x, int32 y) : X(x), Y(y)
{}

FString FTileCoord::ToString()
{
    return FString("X: " + FString::FromInt(X) + " Y: " + FString::FromInt(Y));
}

bool FTileCoord::operator==(const FTileCoord& Coord) const
{
    return X == Coord.X && Y == Coord.Y;
}

bool FTileCoord::operator!=(const FTileCoord& Coord) const
{
    return X != Coord.X || Y != Coord.Y;
}

FTileCoord FTileCoord::operator+(const FTileCoord Bias) const
{
    return FTileCoord(X + Bias.X, Y + Bias.Y);
}

FTileCoord FTileCoord::operator-(const FTileCoord Bias) const
{
    return FTileCoord(X - Bias.X, Y - Bias.Y);
}

FDimensionsDisplayedArea::FDimensionsDisplayedArea(FTileCoord min, FTileCoord max) : Min(min), Max(max)
{}

FString FDimensionsDisplayedArea::ToString()
{
    return FString("Min(" + Min.ToString() + "), Max(" + Max.ToString() + ")");
}

bool FDimensionsDisplayedArea::IsEmpty()
{
    return Min.X == 0 && Min.Y == 0 && Max.X == 0 && Max.Y == 0;
}

bool FDimensionsDisplayedArea::operator==(const FDimensionsDisplayedArea& Dimensions) const
{
    return Min == Dimensions.Min && Max == Dimensions.Max;
}

bool FDimensionsDisplayedArea::operator!=(const FDimensionsDisplayedArea& Dimensions) const
{
    return Min != Dimensions.Min || Max != Dimensions.Max;
}

FDimensionsDisplayedArea FDimensionsDisplayedArea::operator+(const FDimensionsDisplayedArea Bias) const
{
    return FDimensionsDisplayedArea(Min + Bias.Min, Max + Bias.Max);
}

FDimensionsDisplayedArea FDimensionsDisplayedArea::operator-(const FDimensionsDisplayedArea Bias) const
{
    return FDimensionsDisplayedArea(Min - Bias.Min, Max - Bias.Max);
}
