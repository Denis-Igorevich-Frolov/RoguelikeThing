// Fill out your copyright notice in the Description page of Project Settings.


#include "RoguelikeThing/Function Libraries/TileTablesOptimizationTools.h"

void UTileTablesOptimizationTools::InitMapTiles(UUniformGridPanel* TileGridPanel,
    FVector2D TileSize, FVector2D WidgetAreaSize, FMapDimensions MapDimensions)
{
    TileLen = MapDimensions.MapTileLength;
    TableRows = (MapDimensions.MaxRow - MapDimensions.MinRow + 1) * (MapDimensions.TableLength / TileLen);
    TableCols = (MapDimensions.MaxCol - MapDimensions.MinCol + 1) * (MapDimensions.TableLength / TileLen);

    if (TableRows > 3 * (MapDimensions.TableLength / TileLen))
        TableRows = 3 * (MapDimensions.TableLength / TileLen);
    if (TableCols > 3 * (MapDimensions.TableLength / TileLen))
        TableCols = 3 * (MapDimensions.TableLength / TileLen);

    OriginalTileSize = TileSize;
    widgetAreaSize = WidgetAreaSize + OriginalTileSize * 2;

    OriginalTableSize = OriginalTileSize * FVector2D(TableRows, TableCols);
    FVector2D SizeDifference = OriginalTableSize - widgetAreaSize;

    int NumberOfCollapsedTilesTopAndBottom = (SizeDifference.Y / 2) / OriginalTileSize.Y;
    int NumberOfCollapsedTilesRinhtAndLeft = (SizeDifference.X / 2) / OriginalTileSize.X;

    UE_LOG(LogTemp, Error, TEXT("%d, %d"), NumberOfCollapsedTilesTopAndBottom, NumberOfCollapsedTilesRinhtAndLeft);

    int NumberOfItemsInTable = TileGridPanel->GetAllChildren().Num();
    UE_LOG(LogTemp, Error, TEXT("NumberOfItemsInTable: %d, NumberOfRows: %d, NumberOfCols: %d"), NumberOfItemsInTable, TableRows, TableCols);

    if (TileGridPanel->HasAnyChildren()) {
        CurrentDimensions = FDimensionsDisplayedArea(
            FTileCoord(NumberOfCollapsedTilesRinhtAndLeft, NumberOfCollapsedTilesTopAndBottom),
            FTileCoord(TableRows - NumberOfCollapsedTilesRinhtAndLeft - 1, TableCols - NumberOfCollapsedTilesTopAndBottom - 1));

        for (int row = CurrentDimensions.Min.Y; row <= CurrentDimensions.Max.Y; row++) {
            for (int col = CurrentDimensions.Min.X; col <= CurrentDimensions.Max.X; col++) {
                UWidget* GridPanelElement = TileGridPanel->GetChildAt(row * TableCols + col);
                if (GridPanelElement)
                    GridPanelElement->SetVisibility(ESlateVisibility::Visible);
                else
                    UE_LOG(LogTemp, Error, TEXT("CHORT"));
            }
        }
    }

    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, *CurrentDimensions.ToString());

    UWidget* HiddenWidget = TileGridPanel->GetChildAt(0);
    if (HiddenWidget && HiddenWidget->GetVisibility() == ESlateVisibility::Collapsed)
        HiddenWidget->SetVisibility(ESlateVisibility::Hidden);

    HiddenWidget = TileGridPanel->GetChildAt(NumberOfItemsInTable - 1);
    if (HiddenWidget && HiddenWidget->GetVisibility() == ESlateVisibility::Collapsed)
        HiddenWidget->SetVisibility(ESlateVisibility::Hidden);

    OriginalDimensions = CurrentDimensions;
    OldDimensions = CurrentDimensions;
}

void UTileTablesOptimizationTools::ChangingVisibilityOfMapTiles(UUniformGridPanel* TileGridPanel, FVector2D Bias)
{
    Bias.X *= -1;

    FTileCoord MinBiasCoord;
    FTileCoord MaxBiasCoord;

    MinBiasCoord.X = Bias.X / OriginalTileSize.X;
    MaxBiasCoord.X = Bias.X / OriginalTileSize.X;
    MinBiasCoord.Y = Bias.Y / OriginalTileSize.Y;
    MaxBiasCoord.Y = Bias.Y / OriginalTileSize.Y;

    FDimensionsDisplayedArea BiansDimentions = FDimensionsDisplayedArea(MinBiasCoord, MaxBiasCoord);
    CurrentDimensions = OriginalDimensions + BiansDimentions;

    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, *BiansDimentions.ToString());
    GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, *CurrentDimensions.ToString());

    int NumberOfItemsInTable = TileGridPanel->GetAllChildren().Num();

    if (TileGridPanel->HasAnyChildren()) {
        if (OldDimensions != CurrentDimensions){
            bool NewTopBoundMore = CurrentDimensions.Max.Y > OldDimensions.Max.Y;
            bool NewBottomBoundMore = CurrentDimensions.Min.Y < OldDimensions.Min.Y;
            bool NewLeftBoundMore = CurrentDimensions.Min.X < OldDimensions.Min.X;
            bool NewRightBoundMore = CurrentDimensions.Max.X > OldDimensions.Max.X;

            if (NewTopBoundMore) {
                for (int row = OldDimensions.Max.Y + 1; row <= CurrentDimensions.Max.Y; row++) {
                    for (int col = CurrentDimensions.Min.X; col <= CurrentDimensions.Max.X; col++) {
                        UWidget* GridPanelElement = TileGridPanel->GetChildAt(row * TableCols + col);
                        if (GridPanelElement) {
                            GridPanelElement->SetVisibility(ESlateVisibility::Visible);
                        }
                        else
                            UE_LOG(LogTemp, Error, TEXT("CHORT"));
                    }
                }

                for (int row = OldDimensions.Min.Y; row <= CurrentDimensions.Min.Y - 1; row++) {
                    for (int col = CurrentDimensions.Min.X; col <= CurrentDimensions.Max.X; col++) {
                        UWidget* GridPanelElement = TileGridPanel->GetChildAt(row * TableCols + col);
                        if (GridPanelElement) {
                            GridPanelElement->SetVisibility(ESlateVisibility::Collapsed);
                        }
                        else
                            UE_LOG(LogTemp, Error, TEXT("CHORT"));
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
