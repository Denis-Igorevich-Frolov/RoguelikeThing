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
        }

        return false;
    }
    if (!CellClass) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: CellClass is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();
        }

        return false;
    }
    if (!MapTileClass) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapTileClass is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();
        }

        return false;
    }
    if (!MapEditor) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapEditor is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();
        }

        return false;
    }
    if (!TilesCoordWrapper) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: TilesCoordWrapper is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();
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

        //���� ���������� ����� �������� ���������� � ��������� ������
        AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [MapEditor, NumberOfMapTilesCols, NumberOfMapTilesRows, TableLength,
            MapTileLength, DisplayedColNum, DisplayedRowNum, TileGridPanel,  CellClass, MapTileClass, TilesCoordWrapper, this]() {
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
                                if (LoadingWidget) {
                                    LoadingWidget->LoadingComplete(false);
                                    LoadingWidget->RemoveFromParent();
                                }
                                return false;
                            });
                        }

                        /* ���� ���������� �������� �� ����� ������ �� ��������, ��� � ����� ������� -
                         * ������ ������ ��������� ����� �����, � ��������� - ������ ������ */
                        for (int tileRow = MapTileLength - 1; tileRow >= 0; tileRow--) {
                            for (int tileCol = 0; tileCol < MapTileLength; tileCol++) {
                                UMapCell* Cell = CreateWidget<UMapCell>(MapTile, CellClass);

                                if (!Cell) {
                                    UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: Cell was created with an error"));

                                    //������ �������� �������� � �������� ������ ��� ��� ������� ��� ��� ��� ����������
                                    AsyncTask(ENamedThreads::GameThread, [MapEditor, TileGridPanel, this]() {
                                        if (LoadingWidget) {
                                            LoadingWidget->LoadingComplete(false);
                                            LoadingWidget->RemoveFromParent();
                                        }
                                        return false;
                                    });
                                }

                                //���������� ��������� ������ � GridPanel ������������ � �������� ������ ��� ��� ������� ��� ��� ��� ����������
                                AsyncTask(ENamedThreads::GameThread, [MapTile, Cell, tileRow, tileCol]() {
                                    if (MapTile->GetGridPanel())
                                        MapTile->GetGridPanel()->AddChildToUniformGrid(Cell, tileRow, tileCol);
                                    else
                                        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: Failed to get GridPanel from MapTile"));
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
                        AsyncTask(ENamedThreads::GameThread, [TileGridPanel, MapTile, row, col, TilesCoordWrapper]() {
                            TileGridPanel->AddChildToUniformGrid(MapTile, row, col);
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
                    if (LoadingWidget) {
                        LoadingWidget->LoadingComplete(true);
                        LoadingWidget->RemoveFromParent();
                    }

                    MapEditor->UpdateItemAreaContent();

                    //����� ������� ��������� ����� �������� TileGridPanel ������������ ���������
                    TileGridPanel->SetVisibility(ESlateVisibility::Visible);
                });
        });
    }
    else {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapDimensions is not valid"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(true);
            LoadingWidget->RemoveFromParent();
        }

        return false;
    }

    return true;
}

//���� �� �������� ������ ��������, �� �������� ����� ����� ��� ���������
void UFillingMapWithCells::setLoadWidget(ULoadingWidget* newLoadingWidget)
{
    this->LoadingWidget = newLoadingWidget;
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
