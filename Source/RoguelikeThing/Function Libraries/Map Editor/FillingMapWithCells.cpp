// Fill out your copyright notice in the Description page of Project Settings.


#include "RoguelikeThing/Function Libraries/Map Editor/FillingMapWithCells.h"
#include "RoguelikeThing/Widgets/LoadingWidget.h"
#include "RoguelikeThing/Widgets/MapEditor/MapTile.h"
#include "RoguelikeThing/Widgets/MapEditor/MapCell.h"
#include "RoguelikeThing/Widgets/MapEditor/MapEditor.h"
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(FillingMapWithCells);

UFillingMapWithCells::UFillingMapWithCells()
{
    //��������� GameInstance �� ����
    GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (!GameInstance)
        UE_LOG(FillingMapWithCells, Warning, TEXT("Warning in FillingMapWithCells class in constructor - GameInstance was not retrieved from the world"));
}

/* �������, ����������� ����� �������� �� ������ ��.
 *
 * MapTileClass ����������� ������ ���� �����������
 * ������ UMapTile ��� �� �����, CellClass �����������
 * ������ ���� ����������� ������ UMapCell ��� �� ����� */
void UFillingMapWithCells::FillMapEditorWithCells(FMapDimensions MapDimensions, UUniformGridPanel* TilesGridPanel, UClass* CellClass, UClass* MapTileClass,
    UTileBuffer* TileBuf, UMapEditor* MapEditor, UCoordWrapperOfTable* TilesCoordWrapper, UMapMatrix* MapMatrix, FVector2D WidgetAreaSize)
{
    //������������ ������ ���������� ������ ���� �������� ������ �� ����� ���������� �����
    TilesCoordWrapper->Clear();

    //�������� ������������ ����������
    if (!TilesGridPanel) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: TilesGridPanel is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if(GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return;
    }
    if (!CellClass) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: CellClass is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return;
    }
    if (!MapTileClass) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapTileClass is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return;
    }
    if (!MapEditor) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapEditor is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return;
    }
    if (!TilesCoordWrapper) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: TilesCoordWrapper is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return;
    }
    if (!MapMatrix) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapMatrix is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return;
    }

    /* ����� ����������� ������������ ����������� ������, �� �������� ����� �������� �����.
     * ���������� MapTileClass ����� ��� ������ UClass*, �� ��� ���������� ������ ����
     * ���� ����� ����������� ������ ������������� �� UMapTile. ��� �� ��������
     * ����������� �������� ����� �� ������ ��������� �������, ��� ��� ��� ��������
     * ����� ����������� �����.
     *
     * ��� ������ �������� �������� ���� �������� ������, ������� ����� ����� �����.
     * � ������� dynamic_cast ���������� ���� �� � ����� ������������ ����������� ������ UMapTile */
    UUserWidget* TestGridWidget = CreateWidget<UUserWidget>(TilesGridPanel, MapTileClass);
    if (!dynamic_cast<UMapTile*>(TestGridWidget)) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapFragmentClass was expected to inherit from UMapTile, but its class is %s"), *MapTileClass->GetName());

        //���� ����� ��� �����������, �� �������� ������ �� ����� ���������
        if (TestGridWidget) {
            TestGridWidget->RemoveFromParent();
        }

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return;
    }
    //���� �������� ���� ��������, �� �������� ������ ���������, �� ����� ��� ������ ��� ��������
    if (TestGridWidget) {
        TestGridWidget->RemoveFromParent();
    }

    /* ����� ����������� ������������ ����������� ������, �� �������� ����� �������� ������.
     * ���������� CellClass ����� ��� ������ UClass*, �� ��� ���������� ������ ����
     * ���� ����� ����������� ������ ������������� �� UMapCell. ��� �� ��������
     * ����������� �������� ����� �� ������ ��������� �������, ��� ��� ��� ��������
     * ����� ����������� �����.
     *
     * ��� ������ �������� �������� ���� �������� ������, ������� ����� ����� �����.
     * � ������� dynamic_cast ���������� ���� �� � ����� ������������ ����������� ������ UMapCell */
    UUserWidget* TestCellWidget = CreateWidget<UUserWidget>(TilesGridPanel, CellClass);
    if (!StaticCast<UMapCell*>(TestCellWidget)) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: CellClass was expected to inherit from UMapCell, but its class is %s"), *CellClass->GetName());

        //���� ����� ��� �����������, �� �������� ������ �� ����� ���������
        if (TestCellWidget) {
            TestCellWidget->RemoveFromParent();
        }

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return;
    }
    /* �������� ������ ������ ���� �� ���������, �� ��� ���������� ��� ���������� �������� �����. ��������� ����� ������ � ��������
     * ������ ������ ��� ����� ��� ��������� ������ � ��������������� ��������, ��� ������ - ��� ������ � ��� ���������� ����������,
     * � �� ����� ��� ������, ��������� � �������� ���������� ����� ����� ���� �� ��������� �����������, �������� ����� ���� ���������
     * ��������� ������ ����� ����� ��� �����, �� � �������� �� �� ��������� ���������� ������ � ������� ����� ��������� StaticCast'��. */

    if (MapDimensions.isValid) {
        //����� �� ������ ��������� ������ ��� �� ����� ����� �������� ��������� �������� �����, ������� ������� TilesGridPanel ���������������
        TilesGridPanel->SetVisibility(ESlateVisibility::Collapsed);

        int TableLength = MapDimensions.TableLength;
        int MapTileLength = MapDimensions.MapTileLength;

        //�������� ������ ���������� ����� (� ����������)
        ColsNum = MapDimensions.MaxCol - MapDimensions.MinCol + 1;
        RowsNum = MapDimensions.MaxRow - MapDimensions.MinRow + 1;

        int NumberOfMapTilesCols = ColsNum * (TableLength / MapTileLength);
        int NumberOfMapTilesRows = RowsNum * (TableLength / MapTileLength);

        if (TestCellWidget) {
            FVector2D CellSize = static_cast<UMapCell*>(TestCellWidget)->getSize();
            TileSize = CellSize * MapTileLength;
        }
        //� ������ ������ ��������� �������� ������ ������
        if (TestCellWidget) {
            TestCellWidget->RemoveFromParent();
        }

        //���������� ������, ��������� �� �����
        NumberOfTilesColsThatFitOnScreen = ceil((WidgetAreaSize.X) / TileSize.X);
        NumberOfTilesRowsThatFitOnScreen = ceil((WidgetAreaSize.Y) / TileSize.Y);

        //���� ���������� ������, ��������� �� �����, ������ ������������ ���������� ������, �� ��� ���������
        if (NumberOfTilesColsThatFitOnScreen > NumberOfMapTilesCols)
            NumberOfTilesColsThatFitOnScreen = NumberOfMapTilesCols;
        if (NumberOfTilesRowsThatFitOnScreen > NumberOfMapTilesRows)
            NumberOfTilesRowsThatFitOnScreen = NumberOfMapTilesRows;

        //����������, ���������� �������� ������������ ���������� ������
        bool IsNumberOfTilesColsEven = (NumberOfMapTilesCols % 2) == 0;
        bool IsNumberOfTilesRowsEven = (NumberOfMapTilesRows % 2) == 0;

        //����������, ���������� �������� ���������� �� ����� ���������� ������
        bool IsNumberOfFittingTilesColsEven = (NumberOfTilesColsThatFitOnScreen % 2) == 0;
        bool IsNumberOfFittingTilesRowsEven = (NumberOfTilesRowsThatFitOnScreen % 2) == 0;

        /* ׸������ ������������ � ���������� �� ����� ���������� ������ ������ ������ ��������� ��� ����������
         * ��������� �������. ��� ����������� ��������, ���������� ��������� �� ���� ����� ������������� �� 1.
         * ��� ��� ���� ��� ���� ��������� ��������� ��������, �� ������ ���������� ���������� ����� ���������
         * �����������, � ���� ������� ��� ���������� ��� ������������ �������� ����� �� �����, �� ��� ����������
         * ���������� ���������� ����� �� 1, ���������� ������������ ���������� ������� �� ��������. */
        if (IsNumberOfTilesColsEven != IsNumberOfFittingTilesColsEven)
            NumberOfTilesColsThatFitOnScreen++;
        if (IsNumberOfTilesRowsEven != IsNumberOfFittingTilesRowsEven)
            NumberOfTilesRowsThatFitOnScreen++;

        int StartingPositionRow = (NumberOfMapTilesRows - NumberOfTilesRowsThatFitOnScreen) / 2;
        int StartingPositionCol = (NumberOfMapTilesCols - NumberOfTilesColsThatFitOnScreen) / 2;

        if (GameInstance && GameInstance->LogType != ELogType::NONE)
            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: all checks have been passed, the TilesGridPanel grid is ready to be filled with cells with dimensions: rows: %d, columns: %d"), NumberOfMapTilesRows, NumberOfMapTilesCols);

        TileBuf->Clear();
        if (!TileBuf->ScoreToMaximum())
            UE_LOG(FillingMapWithCells, Log, TEXT("fffffffffffff"));

        //���� ���������� ����� �������� ���������� � ��������� ������
        AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [MapEditor, TableLength, TestCellWidget, MapTileLength,TilesGridPanel, TileBuf,
            CellClass, MapTileClass, TilesCoordWrapper, MapMatrix, StartingPositionRow, StartingPositionCol, NumberOfMapTilesRows, NumberOfMapTilesCols, this]() {

                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The TilesGridPanel table population thread has been opened"));

                MapMatrix->FillTerrainOfTiles();

                UMapTile* LastMapTile = nullptr;

                MinOriginalVisibleTile = FGridCoord(StartingPositionRow, StartingPositionCol);
                MaxOriginalVisibleTile = FGridCoord(StartingPositionRow + (NumberOfTilesRowsThatFitOnScreen - 1), StartingPositionCol + (NumberOfTilesColsThatFitOnScreen - 1));

                /* ������ ���� � ��������� � ���� ������� �����. ��� ����
                 * ��� ���������� ����� �������, ����� ����� ������ ����
                 * � ���������� �������� ��� ����� �����, � ��������� �
                 * ���������� - ������ ������.
                 * 
                 * ��� ����� ������ ������������ �������� ������ - 
                 * �� �� ���������� (��� ����� ����� ���������� 
                 * � ������ ���������� �������) �� ����.
                 * ��� �������� �� ������� ������� �� ���� �� ��
                 * ����������, ��� ����, ��-�� ����, ��� ����� <,
                 * � �� <=, ���������� � ������� �� ���������, ���
                 * ��� ��������� ����� �� ����� ��������� */
                for (int row = StartingPositionRow + NumberOfTilesRowsThatFitOnScreen - 1; row >= StartingPositionRow; row--) {
                    for (int col = StartingPositionCol; col < NumberOfTilesColsThatFitOnScreen + StartingPositionCol; col++) {
                        //����� ������� �� ������, �������, � ���� �������, ������� �� �����, ��� ��� ������� �������� ����
                        UMapTile* MapTile = StaticCast<UMapTile*>(CreateWidget<UUserWidget>(TilesGridPanel, MapTileClass));

                        if (!MapTile) {
                            UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapTile was created with an error"));

                            //������ �������� �������� � �������� ������ ��� ��� ������� ��� ��� ��� ����������
                            AsyncTask(ENamedThreads::GameThread, [MapEditor, TilesGridPanel, this]() {
                                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Launched a GameThread call from the TilesGridPanel table population thread to remove the loading widget"));

                                if (LoadingWidget) {
                                    LoadingWidget->LoadingComplete(false);
                                    LoadingWidget->RemoveFromParent();

                                    if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
                                }
                                else
                                    if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: There was no download widget"));

                                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: A thread to remove the loading widget has been closed"));

                                return;
                                });
                        }

                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: An uninitialized MapTile is created with coordinates row: %d col: %d"), row, col);

                        MapTile->SetMyCoord(FCellCoord(row, col));
                        MapTile->SetMyTerrainOfTile(MapMatrix->GetTerrainOfTile(FCellCoord(row, col)));

                        if (!MapTile->FillingWithCells(MapTileLength, CellClass, MapEditor, MapMatrix)) {
                            AsyncTask(ENamedThreads::GameThread, [MapEditor, TilesGridPanel, this]() {
                                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Launched a GameThread call from the TilesGridPanel table population thread to remove the loading widget"));

                                if (LoadingWidget) {
                                    LoadingWidget->LoadingComplete(false);
                                    LoadingWidget->RemoveFromParent();

                                    if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
                                }
                                else
                                    if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: There was no download widget"));

                                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: A thread to remove the loading widget has been closed"));

                                return;
                                });
                        }

                        //���������� ���������� ����� � GridPanel ������������ � �������� ������ ��� ��� ������� ��� ��� ��� ����������
                        AsyncTask(ENamedThreads::GameThread, [TilesGridPanel, MapTile, row, col, TilesCoordWrapper, NumberOfMapTilesRows, this]() {
                            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Launched a GameThread call from the TilesGridPanel table population thread to place a tile in a TilesGridPanel table"));

                            UUniformGridSlot* GridSlot = TilesGridPanel->AddChildToUniformGrid(MapTile, row, col);

                            if (GameInstance && GameInstance->LogType == ELogType::DETAILED) {
                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The tile was placed in a TilesGridPanel table at position col: %d row: %d"), row, col);
                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: A thread to place a tile in a TilesGridPanel table has been closed"));
                            }

                            /* ��������� ����� ���������� � ������������ ������. ��� ���� ����� �� ������ � �������
                             * ��������, �����, ��������, ����� ����� ��� ���� 0x0, � ������ ������ - 10x10.
                             * ��� ����� ������ ��������� ����� �� ������� ���������������, ����� ��������
                             * ��������������� �������� �� ����, ��� ���� ��� ������������� ��� �����������
                             * ������������ ������ � GridPanel */
                            TilesCoordWrapper->AddWidget(NumberOfMapTilesRows - row - 1, col, MapTile, GridSlot);
                        });

                        LastMapTile = MapTile;
                    }
                }

                UMapTile* MaxTile = StaticCast<UMapTile*>(CreateWidget<UUserWidget>(TilesGridPanel, MapTileClass));

                AsyncTask(ENamedThreads::GameThread, [TilesGridPanel, MaxTile, NumberOfMapTilesRows, NumberOfMapTilesCols, this]() {
                    MinTileGridSlot = TilesGridPanel->AddChildToUniformGrid(MaxTile, NumberOfMapTilesRows - 1, NumberOfMapTilesCols - 1);
                });

                //����� ������� �� ������� ��������������� ����������� ������ �����
                TilesGridPanel->SetMinDesiredSlotWidth(TileSize.X);
                TilesGridPanel->SetMinDesiredSlotHeight(TileSize.Y);

                //��� ��������������, ��������� � ���������� ��������� �������� ����������� � �������� ������
                AsyncTask(ENamedThreads::GameThread, [MapEditor, TilesGridPanel, this]() {
                    if (GameInstance && GameInstance->LogType != ELogType::NONE)
                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Launched a GameThread call from the TilesGridPanel table population thread to configure the necessary widgets at the end of filling the TilesGridPanel table and removing the loading widget"));

                    if (LoadingWidget) {
                        LoadingWidget->LoadingComplete(true);
                        LoadingWidget->RemoveFromParent();

                        if (GameInstance && GameInstance->LogType != ELogType::NONE)
                            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
                    }
                    else
                        if (GameInstance && GameInstance->LogType != ELogType::NONE)
                            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: There was no download widget"));

                    MapEditor->UpdateItemAreaContent();

                    //����� ������� ��������� ����� �������� TilesGridPanel ������������ ���������
                    TilesGridPanel->SetVisibility(ESlateVisibility::Visible);
                    if (GameInstance && GameInstance->LogType != ELogType::NONE)
                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: A thread to configure the necessary widgets at the end of filling the TilesGridPanel table and removing the loading widget has been closed"));
                });

                AsyncTask(ENamedThreads::GameThread, [this]() {
                    GetWorld()->ForceGarbageCollection(true);
                    });
        });
    }
    else {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapDimensions is not valid"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(true);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return;
    }
}

//���� �� �������� ������ ��������, �� �������� ����� ����� ��� ���������
void UFillingMapWithCells::setLoadWidget(ULoadingWidget* newLoadingWidget)
{
    this->LoadingWidget = newLoadingWidget;

    if (GameInstance && GameInstance->LogType != ELogType::NONE)
        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the setLoadWidget function: The download widget has been set"));
}

//������ ���������� ���������� �� �����������
int32 UFillingMapWithCells::GetColNum()
{
    return ColsNum;
}

//������ ���������� ���������� �� ���������
int32 UFillingMapWithCells::GetRowNum()
{
    return RowsNum;
}
