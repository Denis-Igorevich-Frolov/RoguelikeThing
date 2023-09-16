// Fill out your copyright notice in the Description page of Project Settings.


#include "RoguelikeThing/Function Libraries/Map Editor/FillingMapWithCells.h"
#include "RoguelikeThing/Widgets/LoadingWidget.h"
#include "RoguelikeThing/Widgets/MapEditor/MapTile.h"
#include "RoguelikeThing/Widgets/MapEditor/MapCell.h"
#include "RoguelikeThing/Widgets/MapEditor/MapEditor.h"
#include "Components/CanvasPanelSlot.h"
#include <Blueprint/WidgetLayoutLibrary.h>

DEFINE_LOG_CATEGORY(FillingMapWithCells);

bool UFillingMapWithCells::FillMapEditorWithCells(FMapDimensions MapDimensions,
    UUniformGridPanel* TileGridPanel, UClass* CellClass, UClass* MapTileClass, UMapEditor* MapEditor)
{
    if (!TileGridPanel)
        return false;
    if (!CellClass)
        return false;
    if (!MapTileClass)
        return false;
    if (!MapEditor)
        return false;

    UUserWidget* TestGridWidget = CreateWidget<UUserWidget>(TileGridPanel, MapTileClass);
    if (!dynamic_cast<UMapTile*>(TestGridWidget)) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapFragmentClass was expected to inherit from UMapTile, but its class is %s"), *MapTileClass->GetName());

        if (TestGridWidget) {
            TestGridWidget->RemoveFromParent();
        }

        if (LoadingWidget) {
            LoadingWidget->RemoveFromParent();
            LoadingWidget->LoadingComplete(false);
        }

        return false;
    }
    if (TestGridWidget) {
        TestGridWidget->RemoveFromParent();
    }

    UUserWidget* TestCellWidget = CreateWidget<UUserWidget>(TileGridPanel, CellClass);
    if (!StaticCast<UMapCell*>(TestCellWidget)) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: CellClass was expected to inherit from UMapCell, but its class is %s"), *CellClass->GetName());

        if (TestCellWidget) {
            TestCellWidget->RemoveFromParent();
        }

        if (LoadingWidget) {
            LoadingWidget->RemoveFromParent();
            LoadingWidget->LoadingComplete(false);
        }

        return false;
    }
    if (TestCellWidget) {
        TestCellWidget->RemoveFromParent();
    }

    if (MapDimensions.isValid) {
        TileGridPanel->SetVisibility(ESlateVisibility::Collapsed);

        int TableLength = MapDimensions.TableLength;
        int MapTileLength = MapDimensions.MapTileLength;

        ColSize = MapDimensions.MaxCol - MapDimensions.MinCol + 1;
        RowSize = MapDimensions.MaxRow - MapDimensions.MinRow + 1;

        int DisplayedColSize = ColSize;
        int DisplayedRowSize = RowSize;

        if (DisplayedColSize > 3)
            DisplayedColSize = 3;
        if (DisplayedRowSize > 3)
            DisplayedRowSize = 3;

        int NumberOfMapTilesCols = DisplayedColSize * TableLength / MapTileLength;
        int NumberOfMapTilesRows = DisplayedRowSize * TableLength / MapTileLength;


        AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [MapEditor, NumberOfMapTilesCols, NumberOfMapTilesRows,
            TableLength, MapTileLength, DisplayedColSize, DisplayedRowSize, TileGridPanel,  CellClass, MapTileClass, this]() {
                FVector2D TileSize(0, 0);
                for (int col = 0; (col < NumberOfMapTilesCols); col++) {
                    for (int row = 0; (row < NumberOfMapTilesRows); row++) {
                        UMapTile* MapTile = StaticCast<UMapTile*>(CreateWidget<UUserWidget>(TileGridPanel, MapTileClass));

                        for (int tileCol = 0; tileCol < MapTileLength; tileCol++) {
                            for (int tileRow = 0; tileRow < MapTileLength; tileRow++) {
                                UMapCell* Cell = CreateWidget<UMapCell>(MapTile, CellClass);

                                AsyncTask(ENamedThreads::GameThread, [MapTile, Cell, tileRow, tileCol]() {
                                    if (MapTile->GetGridPanel())
                                        MapTile->GetGridPanel()->AddChildToUniformGrid(Cell, tileRow, tileCol);
                                    else
                                        UE_LOG(FillingMapWithCells, Error, TEXT("ERROR"));
                                });

                                if (tileCol == MapTileLength - 1 && tileRow == MapTileLength - 1) {
                                    TileSize = Cell->getSize() * MapTileLength;
                                    UWidgetLayoutLibrary::SlotAsCanvasSlot(MapTile->GetGridPanel())->SetSize(TileSize);
                                }
                            }
                        }

                        AsyncTask(ENamedThreads::GameThread, [TileGridPanel, MapTile, row, col]() {
                                TileGridPanel->AddChildToUniformGrid(MapTile, row, col);
                        });
                    }
                }

                TileGridPanel->SetMinDesiredSlotWidth(TileSize.X);
                TileGridPanel->SetMinDesiredSlotHeight(TileSize.Y);

                AsyncTask(ENamedThreads::GameThread, [MapEditor, TileGridPanel, this]() {
                    if (LoadingWidget) {
                        LoadingWidget->RemoveFromParent();
                        LoadingWidget->LoadingComplete(true);

                        MapEditor->UpdateMapSize();

                        TileGridPanel->SetVisibility(ESlateVisibility::Visible);
                    }
                });
        });
    }
    else {
        return false;
    }

    return true;
}

void UFillingMapWithCells::setLoadWidget(ULoadingWidget* newLoadingWidget)
{
    this->LoadingWidget = newLoadingWidget;
}

int32 UFillingMapWithCells::GetColSize()
{
    return ColSize;
}

int32 UFillingMapWithCells::GetRowSize()
{
    return RowSize;
}
