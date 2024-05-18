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
bool UFillingMapWithCells::FillMapEditorWithCells(FMapDimensions MapDimensions, UUniformGridPanel* TilesGridPanel, UClass* CellClass,
    UClass* MapTileClass, UMapEditor* MapEditor, UCoordWrapperOfTable* TilesCoordWrapper, UMapMatrix* MapMatrix)
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

        return false;
    }
    if (!CellClass) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: CellClass is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return false;
    }
    if (!MapTileClass) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapTileClass is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return false;
    }
    if (!MapEditor) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapEditor is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return false;
    }
    if (!TilesCoordWrapper) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: TilesCoordWrapper is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return false;
    }
    if (!MapMatrix) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapMatrix is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
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

        return false;
    }
    //���� �������� ���� ��������, �� �������� ������ ���������, �� ����� ��� ������ ��� ��������
    if (TestCellWidget) {
        TestCellWidget->RemoveFromParent();
    }

    if (MapDimensions.isValid) {
        //����� �� ������ ��������� ������ ��� �� ����� ����� �������� ��������� �������� �����, ������� ������� TilesGridPanel ���������������
        TilesGridPanel->SetVisibility(ESlateVisibility::Collapsed);

        int TableLength = MapDimensions.TableLength;
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

        if (GameInstance && GameInstance->LogType != ELogType::NONE)
            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: all checks have been passed, the TilesGridPanel grid is ready to be filled with cells with dimensions: rows: %d, columns: %d"), NumberOfMapTilesRows, NumberOfMapTilesCols);

        //���� ���������� ����� �������� ���������� � ��������� ������
        AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [MapEditor, NumberOfMapTilesCols, NumberOfMapTilesRows, TableLength,
            MapTileLength, DisplayedColNum, DisplayedRowNum, TilesGridPanel,  CellClass, MapTileClass, TilesCoordWrapper, MapMatrix, this]() {

                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The TilesGridPanel table population thread has been opened"));

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

                                return false;
                                });
                        }

                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: An uninitialized MapTile is created with coordinates row: %d col: %d"), row, col);

                        /* ���� ���������� �������� �� ����� ������ �� ��������, ��� � ����� ������� -
                         * ������ ������ ��������� ����� �����, � ��������� - ������ ������ */
                        for (int tileRow = MapTileLength - 1; tileRow >= 0; tileRow--) {
                            for (int tileCol = 0; tileCol < MapTileLength; tileCol++) {
                                UMapCell* Cell = CreateWidget<UMapCell>(MapTile, CellClass);

                                if (!Cell) {
                                    UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: Cell was created with an error"));

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

                                        return false;
                                        });
                                }

                                //������ ��������� � ���������� ����������, ������ � ����������� ������ �� ��������� � ����������� (����� ������� � 1)
                                Cell->MyCoord = FCellCoord(row * MapTileLength + tileRow + 1, col * MapTileLength + tileCol + 1);
                                //����� �� ��������� ��������� �� � �������� ����
                                Cell->MyEditor = MapEditor;

                                if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                                    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: An uninitialized cell is created with coordinates row: %d col: %d for a MapTile with coordinates row: %d col: %d"), tileRow, tileCol, row, col);

                                //���������� ��������� ������ � GridPanel ������������ � �������� ������ ��� ��� ������� ��� ��� ��� ����������
                                AsyncTask(ENamedThreads::GameThread, [MapTile, Cell, tileRow, tileCol, row, col, this]() {
                                    if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Launched a GameThread call from the TilesGridPanel table population thread to place a cell in a tile"));

                                    if (MapTile->GetGridPanel()) {
                                        MapTile->GetGridPanel()->AddChildToUniformGrid(Cell, tileRow, tileCol);

                                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                                            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The cell was placed in a tile located at coordinates col: %d row: %d, at position col: %d row: %d"), tileRow, tileCol, row, col);
                                    }
                                    else
                                        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: Failed to get GridPanel from MapTile"));

                                    if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: A thread to place a cell in a tile has been closed"));
                                    });

                                //������ ������ ����� ������� ����� ������ ����������� �����. ��� ����� �� �� �������� ��� ��������� ������
                                FMapEditorBrushType CellType = MapMatrix->GetValueOfMapChunkStructureCellByGlobalIndex(row * MapTileLength + tileRow + 1, col * MapTileLength + tileCol + 1, false);

                                /* � ����� � ������������ � ��������� �����, ������ ������������� ����������� �����.
                                 * ��� ���� ������ ����� ��������� �� ����, �� � ��� �������� ������ �� ��������� */
                                switch (CellType)
                                {
                                case FMapEditorBrushType::Corridor:
                                    Cell->SetCorridorStyle(MapMatrix->CheckNeighbourhoodOfCell(MatrixType::ChunkStructure, row * MapTileLength + tileRow + 1, col * MapTileLength + tileCol + 1, false));
                                    if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Cell Row: %d Col: %d is set to the corridor style"), row * MapTileLength + tileRow + 1, col * MapTileLength + tileCol + 1);
                                    break;
                                case FMapEditorBrushType::Room:
                                    Cell->SetRoomStyle();
                                    if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Cell Row: %d Col: %d is set to the room style"), row * MapTileLength + tileRow + 1, col * MapTileLength + tileCol + 1);
                                    break;
                                default:
                                    break;
                                }

                                /* ��������� ������ ���������� � ������������ ������. ��� ���� ����� �� ������ � �������
                                 * ��������, �����, ��������, ����� ����� ���� ������ 0x0, � ������ ������ - 5x5.
                                 * ��� ����� ������ ��������� ����� �� ������� ���������������, ����� ��������
                                 * ��������������� �������� �� ����, ��� ���� ��� ������������� ��� �����������
                                 * ������������ ����� � GridPanel */
                                MapTile->CellsCoordWrapper->AddWidget(MapTileLength - 1 - tileRow, tileCol, Cell);
                            }
                        }

                        if (MapTile->GetGridPanel()->HasAnyChildren()) {
                            if (dynamic_cast<UMapCell*>(MapTile->GetGridPanel()->GetChildAt(0))) {
                                TileSize = static_cast<UMapCell*>(MapTile->GetGridPanel()->GetChildAt(0))->getSize() * MapTileLength;
                            }
                            else {
                                UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: The MapTile child cell at index 0 is not a UMapCell* or is not valid"));
                                AsyncTask(ENamedThreads::GameThread, [this]() {
                                    if (LoadingWidget) {
                                        LoadingWidget->LoadingComplete(true);
                                        LoadingWidget->RemoveFromParent();

                                        if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
                                    }

                                    return false;
                                });
                            }
                        }
                        else {
                            UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapTile does not have any children"));
                        
                            AsyncTask(ENamedThreads::GameThread, [this]() {
                                if (LoadingWidget) {
                                    LoadingWidget->LoadingComplete(true);
                                    LoadingWidget->RemoveFromParent();

                                    if (GameInstance && GameInstance->LogType != ELogType::NONE)
                                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
                                }

                                return false;
                            });
                        }

                        //���������� ���������� ����� � GridPanel ������������ � �������� ������ ��� ��� ������� ��� ��� ��� ����������
                        AsyncTask(ENamedThreads::GameThread, [TilesGridPanel, MapTile, row, col, TilesCoordWrapper, this]() {
                            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Launched a GameThread call from the TilesGridPanel table population thread to place a tile in a TilesGridPanel table"));

                            TilesGridPanel->AddChildToUniformGrid(MapTile, row, col);

                            if (GameInstance && GameInstance->LogType == ELogType::DETAILED) {
                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The tile was placed in a TilesGridPanel table at position col: %d row: %d"), row, col);
                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: A thread to place a tile in a TilesGridPanel table has been closed"));
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

        return false;
    }

    return true;
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
    return ColNum;
}

//������ ���������� ���������� �� ���������
int32 UFillingMapWithCells::GetRowNum()
{
    return RowNum;
}
