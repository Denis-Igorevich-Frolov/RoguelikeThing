// Fill out your copyright notice in the Description page of Project Settings.


#include "RoguelikeThing/Function Libraries/Map Editor/FillingMapWithCells.h"
#include "RoguelikeThing/Widgets/LoadingWidget.h"

void UFillingMapWithCells::FillMapEditorWithCells(FMapDimensions MapDimensions, int32 TableLength, UUniformGridPanel* GridPanel, UUserWidget* SampleWidget)
{
    if (MapDimensions.isValid) {
        GridPanel->SetVisibility(ESlateVisibility::Collapsed);

        int32 ColSize = MapDimensions.MaxCol - MapDimensions.MinCol + 1;
        int32 RowSize = MapDimensions.MaxRow - MapDimensions.MinRow + 1;
        
        AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [ColSize, RowSize, TableLength, GridPanel, SampleWidget, this]() {
            for (int col = 0; (col < (ColSize * TableLength)); col++) {
                for (int row = 0; (row < (RowSize * TableLength)); row++) {
                    UUserWidget* Cell = CreateWidget<UUserWidget>(GridPanel, SampleWidget->GetClass());
                    AsyncTask(ENamedThreads::GameThread, [GridPanel, Cell, row, col, ColSize, RowSize, TableLength, this]() {
                        GridPanel->AddChildToUniformGrid(Cell, row, col);
                        if (col == (ColSize * TableLength) - 1 && row == (RowSize * TableLength) - 1) {
                            AsyncTask(ENamedThreads::GameThread, [this, GridPanel]() {
                                if (LoadingWidget) {
                                    LoadingWidget->RemoveFromParent();
                                    LoadingWidget->LoadingComplete(true);
                                    GridPanel->SetVisibility(ESlateVisibility::Visible);
                                }
                            });
                        }
                    });
                }
            }
        });
    }
}

void UFillingMapWithCells::setLoadWidget(ULoadingWidget* newLoadingWidget)
{
    this->LoadingWidget = newLoadingWidget;
}
