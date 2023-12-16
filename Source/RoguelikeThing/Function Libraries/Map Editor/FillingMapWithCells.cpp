// Fill out your copyright notice in the Description page of Project Settings.


#include "RoguelikeThing/Function Libraries/Map Editor/FillingMapWithCells.h"
#include "RoguelikeThing/Widgets/LoadingWidget.h"
#include "RoguelikeThing/Widgets/MapEditor/MapTile.h"
#include "RoguelikeThing/Widgets/MapEditor/MapCell.h"
#include "RoguelikeThing/Widgets/MapEditor/MapEditor.h"

DEFINE_LOG_CATEGORY(FillingMapWithCells);

/* �������, ����������� ������� �������� �����.
 *
 * MapTileClass ����������� ������ ���� �����������
 * ������ UMapTile ��� �� �����, CellClass �����������
 * ������ ���� ����������� ������ UMapCell ��� �� ����� */
bool UFillingMapWithCells::FillMapEditorWithCells(FMapDimensions MapDimensions, UUniformGridPanel* TileGridPanel,
    UClass* CellClass, UClass* MapTileClass, UMapEditor* MapEditor, UCoordWrapperOfTable* TilesCoordWrapper)
{
    //������������ ������ ���������� ������ ���� �������� ������ �� ����� ���������� �����
    TilesCoordWrapper->Clear();

    //�������� ������������ ����������
    if (!TileGridPanel) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: TileGridPanel is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return false;
    }
    if (!CellClass) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: CellClass is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return false;
    }
    if (!MapTileClass) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapTileClass is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return false;
    }
    if (!MapEditor) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapEditor is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return false;
    }
    if (!TilesCoordWrapper) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: TilesCoordWrapper is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return false;
    }

    /* ����� ����������� ������������ ����������� ������, �� �������� ����� �������� �����.
     * ���������� MapTileClass ����� ��� ������ UClass*, �� ��� ���������� ������ ����
     * ���� ����� ����������� ������ ������������� �� UMapTile. ��� �� ��������
     * ����������� �������� ����� �� ������ ��������� �������, ��� ��� ��� ��������
     * ����� ����������� �����.
     *
     * ��� ������ �������� �������� ���� �������� ������, ������� ����� ����� �����.
     * � ������� dynamic_cast ���������� ���� �� � ����� ������������ ����������� ������ UMapTile */
    UUserWidget* TestGridWidget = CreateWidget<UUserWidget>(TileGridPanel, MapTileClass);
    if (!dynamic_cast<UMapTile*>(TestGridWidget)) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapFragmentClass was expected to inherit from UMapTile, but its class is %s"), *MapTileClass->GetName());

        //���� ����� ��� �����������, �� �������� ������ �� ����� ���������
        if (TestGridWidget) {
            TestGridWidget->RemoveFromParent();
        }

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return false;
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
    UUserWidget* TestCellWidget = CreateWidget<UUserWidget>(TileGridPanel, CellClass);
    if (!StaticCast<UMapCell*>(TestCellWidget)) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: CellClass was expected to inherit from UMapCell, but its class is %s"), *CellClass->GetName());

        //���� ����� ��� �����������, �� �������� ������ �� ����� ���������
        if (TestCellWidget) {
            TestCellWidget->RemoveFromParent();
        }

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return false;
    }
    //���� �������� ���� ��������, �� �������� ������ ���������, �� ����� ��� ������ ��� ��������
    if (TestCellWidget) {
        TestCellWidget->RemoveFromParent();
    }

    if (MapDimensions.isValid) {
        //����� �� ������ ��������� ������ ��� �� ����� ����� �������� ��������� �������� �����, ������� ������� TileGridPanel ���������������
        TileGridPanel->SetVisibility(ESlateVisibility::Collapsed);

        //����� ��������� �����, ������������� �������� ������, ������� ���������� �����
        int TableLength = MapDimensions.TableLength;
        //������ ����� �����, ������� ��������� ������ ��� �����������
        int MapTileLength = MapDimensions.MapTileLength;

        //�������� ������ ���������� ����� (� ����������)
        ColNum = MapDimensions.MaxCol - MapDimensions.MinCol + 1;
        RowNum = MapDimensions.MaxRow - MapDimensions.MinRow + 1;

        //������������ ������ �����, ������� ������� �� ��������� 3 ��������� ��� ������ ������������������
        int DisplayedColNum = ColNum;
        int DisplayedRowNum = RowNum;

        if (DisplayedColNum > 3)
            DisplayedColNum = 3;
        if (DisplayedRowNum > 3)
            DisplayedRowNum = 3;

        //������ ������������ ����� (� ������)
        int NumberOfMapTilesCols = DisplayedColNum * TableLength / MapTileLength;
        int NumberOfMapTilesRows = DisplayedRowNum * TableLength / MapTileLength;

        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: all checks have been passed, the TileGridPanel grid is ready to be filled with cells with dimensions: rows: %d, columns: %d"), NumberOfMapTilesRows, NumberOfMapTilesCols);

        //���� ���������� ����� �������� ���������� � ��������� ������
        AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [MapEditor, NumberOfMapTilesCols, NumberOfMapTilesRows, TableLength,
            MapTileLength, DisplayedColNum, DisplayedRowNum, TileGridPanel,  CellClass, MapTileClass, TilesCoordWrapper, this]() {

                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The TileGridPanel table population thread has been opened"));

                FVector2D TileSize(0, 0);

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
                for (int row = NumberOfMapTilesRows - 1; row >= 0; row--) {
                    for (int col = 0; col < NumberOfMapTilesCols; col++) {
                        //����� ������� �� ������, �������, � ���� �������, ������� �� �����, ��� ��� ������� �������� ����
                        UMapTile* MapTile = StaticCast<UMapTile*>(CreateWidget<UUserWidget>(TileGridPanel, MapTileClass));

                        if (!MapTile) {
                            UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapTile was created with an error"));

                            //������ �������� �������� � �������� ������ ��� ��� ������� ��� ��� ��� ����������
                            AsyncTask(ENamedThreads::GameThread, [MapEditor, TileGridPanel, this]() {
                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Launched a GameThread call from the TileGridPanel table population thread to remove the loading widget"));

                                if (LoadingWidget) {
                                    LoadingWidget->LoadingComplete(false);
                                    LoadingWidget->RemoveFromParent();

                                    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
                                }
                                else
                                    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: There was no download widget"));

                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: A thread to remove the loading widget has been closed"));

                                return false;
                            });
                        }

                        if(DetailedLogs)
                            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: An uninitialized MapTile is created with coordinates row: %d col: %d"), row, col);

                        /* ���� ���������� �������� �� ����� ������ �� ��������, ��� � ����� ������� -
                         * ������ ������ ��������� ����� �����, � ��������� - ������ ������ */
                        for (int tileRow = MapTileLength - 1; tileRow >= 0; tileRow--) {
                            for (int tileCol = 0; tileCol < MapTileLength; tileCol++) {
                                UMapCell* Cell = CreateWidget<UMapCell>(MapTile, CellClass);

                                if (!Cell) {
                                    UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: Cell was created with an error"));

                                    //������ �������� �������� � �������� ������ ��� ��� ������� ��� ��� ��� ����������
                                    AsyncTask(ENamedThreads::GameThread, [MapEditor, TileGridPanel, this]() {
                                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Launched a GameThread call from the TileGridPanel table population thread to remove the loading widget"));

                                        if (LoadingWidget) {
                                            LoadingWidget->LoadingComplete(false);
                                            LoadingWidget->RemoveFromParent();

                                            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
                                        }
                                        else
                                            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: There was no download widget"));

                                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: A thread to remove the loading widget has been closed"));

                                        return false;
                                    });
                                }

                                if (DetailedLogs)
                                    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: An uninitialized cell is created with coordinates row: %d col: %d for a MapTile with coordinates row: %d col: %d"), tileRow, tileCol, row, col);

                                //���������� ��������� ������ � GridPanel ������������ � �������� ������ ��� ��� ������� ��� ��� ��� ����������
                                AsyncTask(ENamedThreads::GameThread, [MapTile, Cell, tileRow, tileCol, row, col, this]() {
                                    if (DetailedLogs)
                                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Launched a GameThread call from the TileGridPanel table population thread to place a cell in a tile"));

                                    if (MapTile->GetGridPanel()) {
                                        MapTile->GetGridPanel()->AddChildToUniformGrid(Cell, tileRow, tileCol);

                                        if (DetailedLogs)
                                            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The cell was placed in a tile located at coordinates col: %d row: %d, at position col: %d row: %d"), tileRow, tileCol, row, col);
                                    }
                                    else
                                        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: Failed to get GridPanel from MapTile"));

                                    if (DetailedLogs)
                                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: A thread to place a cell in a tile has been closed"));
                                });

                                /* ��� ����������� ���������� ���� ��������� ������ �����.
                                 * � ����� ����� �� ����������� ������ � ����������� TileGridPanel,
                                 * ����� ������ ����������� ���� ������� �� ����� ������ �������� */
                                if (tileRow == 0 && tileCol == MapTileLength - 1) {
                                    //������ ����� ����� ����� ������ ������� ���� �����, ������� �������������� �������� ���� �� ������
                                    TileSize = Cell->getSize() * MapTileLength;
                                }
                            }
                        }

                        //���������� ���������� ����� � GridPanel ������������ � �������� ������ ��� ��� ������� ��� ��� ��� ����������
                        AsyncTask(ENamedThreads::GameThread, [TileGridPanel, MapTile, row, col, TilesCoordWrapper, this]() {
                            if (DetailedLogs)
                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Launched a GameThread call from the TileGridPanel table population thread to place a tile in a TileGridPanel table"));

                            TileGridPanel->AddChildToUniformGrid(MapTile, row, col);

                            if (DetailedLogs) {
                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The tile was placed in a TileGridPanel table at position col: %d row: %d"), row, col);
                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: A thread to place a tile in a TileGridPanel table has been closed"));
                            }
                        });

                        /* ��������� ����� ���������� � ������������ ������. ��� ���� ����� �� ������ � �������
                         * ��������, �����, ��������, ����� ����� ��� ���� 0x0, � ������ ������ - 10x10.
                         * ��� ����� ������ ��������� ����� �� ������� ���������������, ����� ��������
                         * ��������������� �������� �� ����, ��� ���� ��� ������������� ��� �����������
                         * ������������ ������ � GridPanel */
                        TilesCoordWrapper->AddWidget(NumberOfMapTilesRows - 1 - row, col, MapTile);
                    }
                }

                //����� ������� �� ������� ��������������� ����������� ������ �����
                TileGridPanel->SetMinDesiredSlotWidth(TileSize.X);
                TileGridPanel->SetMinDesiredSlotHeight(TileSize.Y);

                //��� ��������������, ��������� � ���������� ��������� �������� ����������� � �������� ������
                AsyncTask(ENamedThreads::GameThread, [MapEditor, TileGridPanel, this]() {
                    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Launched a GameThread call from the TileGridPanel table population thread to configure the necessary widgets at the end of filling the TileGridPanel table and removing the loading widget"));

                    if (LoadingWidget) {
                        LoadingWidget->LoadingComplete(true);
                        LoadingWidget->RemoveFromParent();

                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
                    }
                    else
                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: There was no download widget"));

                    MapEditor->UpdateItemAreaContent();

                    //����� ������� ��������� ����� �������� TileGridPanel ������������ ���������
                    TileGridPanel->SetVisibility(ESlateVisibility::Visible);
                    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: A thread to configure the necessary widgets at the end of filling the TileGridPanel table and removing the loading widget has been closed"));
                });
        });
    }
    else {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapDimensions is not valid"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(true);
            LoadingWidget->RemoveFromParent();

            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return false;
    }

    return true;
}

//���� �� �������� ������ ��������, �� �������� ����� ����� ��� ���������
void UFillingMapWithCells::setLoadWidget(ULoadingWidget* newLoadingWidget)
{
    this->LoadingWidget = newLoadingWidget;

    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the setLoadWidget function: The download widget has been set"));
}

//������ ���������� ���������� �� �����������
int32 UFillingMapWithCells::GetColNum()
{
    return ColNum;
}

//������ ���������� ���������� �� ���������
int32 UFillingMapWithCells::GetRowNum()
{
    return RowNum;
}
