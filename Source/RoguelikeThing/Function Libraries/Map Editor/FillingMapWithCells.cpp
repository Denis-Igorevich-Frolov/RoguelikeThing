// Fill out your copyright notice in the Description page of Project Settings.


#include "RoguelikeThing/Function Libraries/Map Editor/FillingMapWithCells.h"
#include "RoguelikeThing/Widgets/LoadingWidget.h"
#include "RoguelikeThing/Widgets/MapEditor/MapTile.h"
#include "RoguelikeThing/Widgets/MapEditor/MapCell.h"
#include "Components/CanvasPanelSlot.h"
#include <Blueprint/WidgetLayoutLibrary.h>

DEFINE_LOG_CATEGORY(FillingMapWithCells);

bool UFillingMapWithCells::FillMapEditorWithCells(FMapDimensions MapDimensions,
    UUniformGridPanel* GridPanel, UClass* CellClass, UClass* MapTileClass)
{
    UUserWidget* TestGridWidget = CreateWidget<UUserWidget>(GridPanel, MapTileClass);
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

    UUserWidget* TestCellWidget = CreateWidget<UUserWidget>(GridPanel, CellClass);
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
        GridPanel->SetVisibility(ESlateVisibility::Collapsed);

        int TableLength = MapDimensions.TableLength;
        int MapTileLength = MapDimensions.MapTileLength;

        int ColSize = MapDimensions.MaxCol - MapDimensions.MinCol + 1;
        int RowSize = MapDimensions.MaxRow - MapDimensions.MinRow + 1;

        int NumberOfMapFragmentCols = ColSize * TableLength / MapTileLength;
        int NumberOfMapFragmentRows = RowSize * TableLength / MapTileLength;


        AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [NumberOfMapFragmentCols, NumberOfMapFragmentRows,
            TableLength, MapTileLength, ColSize, RowSize, GridPanel,  CellClass, MapTileClass, this]() {
            for (int col = 0; (col < NumberOfMapFragmentCols); col++) {
                for (int row = 0; (row < NumberOfMapFragmentRows); row++) {
                    UMapTile* MapTile = StaticCast<UMapTile*>(CreateWidget<UUserWidget>(GridPanel, MapTileClass));

                    for (int tileCol = 0; tileCol < MapTileLength; tileCol++) {
                        for (int tileRow = 0; tileRow < MapTileLength; tileRow++) {
                            UMapCell* Cell = CreateWidget<UMapCell>(MapTile, CellClass);

                            AsyncTask(ENamedThreads::GameThread, [MapTile, Cell, tileRow, tileCol]() {
                                if (MapTile->GetGridPanel())
                                    MapTile->GetGridPanel()->AddChildToUniformGrid(Cell, tileRow, tileCol);
                                else
                                    UE_LOG(FillingMapWithCells, Error, TEXT("ARROR"));
                            });

                            if (tileCol == MapTileLength-1 && tileRow == MapTileLength - 1) {
                                UWidgetLayoutLibrary::SlotAsCanvasSlot(MapTile->GetGridPanel())->SetSize(Cell->getSize() * MapTileLength);
                            }
                        }
                    }

                    AsyncTask(ENamedThreads::GameThread, [GridPanel, MapTile, row, col,
                        NumberOfMapFragmentCols, NumberOfMapFragmentRows, this]() {
                        GridPanel->AddChildToUniformGrid(MapTile, row, col);
                        if (col == NumberOfMapFragmentCols - 1 && row == NumberOfMapFragmentRows - 1) {
                            if (LoadingWidget) {
                                LoadingWidget->RemoveFromParent();
                                LoadingWidget->LoadingComplete(true);
                                GridPanel->SetVisibility(ESlateVisibility::Visible);
                            }
                        }
                    });
                }
            }
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
