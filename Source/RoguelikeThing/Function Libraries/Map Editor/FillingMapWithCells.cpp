// Fill out your copyright notice in the Description page of Project Settings.


#include "RoguelikeThing/Function Libraries/Map Editor/FillingMapWithCells.h"
#include "RoguelikeThing/Widgets/LoadingWidget.h"
#include "RoguelikeThing/Widgets/MapEditor/MapTile.h"
#include "RoguelikeThing/Widgets/MapEditor/MapCell.h"
#include "RoguelikeThing/Widgets/MapEditor/MapEditor.h"

DEFINE_LOG_CATEGORY(FillingMapWithCells);

/* Функция, заполняющая пустыми ячейками карту.
 *
 * MapTileClass обязательно должен быть наследником
 * класса UMapTile или им самим, CellClass обязательно
 * должен быть наследником класса UMapCell или им самим */
bool UFillingMapWithCells::FillMapEditorWithCells(FMapDimensions MapDimensions, UUniformGridPanel* TileGridPanel,
    UClass* CellClass, UClass* MapTileClass, UMapEditor* MapEditor, UCoordWrapperOfTable* TilesCoordWrapper)
{
    //Координатная обёртка изначально должна быть полность пустой во время заполнения карты
    TilesCoordWrapper->Clear();

    //Проверка корректности указателей
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

    /* Здесь проверяется корректность переданного класса, по которому будут делаться тайлы.
     * Переданный MapTileClass имеет тип данных UClass*, но для корректной работы кода
     * этот класс обязательно должен наследоваться от UMapTile. Мне не известны
     * инструменты проверки этого на уровне аргумента функции, так что эта проверка
     * будет выполняться здесь.
     *
     * Для данной проверки создаётся один тестовый виджет, который после будет удалён.
     * С помощью dynamic_cast выясняется есть ли в древе наследовании переданного класса UMapTile */
    UUserWidget* TestGridWidget = CreateWidget<UUserWidget>(TileGridPanel, MapTileClass);
    if (!dynamic_cast<UMapTile*>(TestGridWidget)) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapFragmentClass was expected to inherit from UMapTile, but its class is %s"), *MapTileClass->GetName());

        //Если класс был некорректен, то тестовый виджет всё равно удаляется
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
    //Если проверка была пройдена, то тестовый виджет удаляется, он нужен был сугубо для проверки
    if (TestGridWidget) {
        TestGridWidget->RemoveFromParent();
    }

    /* Здесь проверяется корректность переданного класса, по которому будут делаться ячейки.
     * Переданный CellClass имеет тип данных UClass*, но для корректной работы кода
     * этот класс обязательно должен наследоваться от UMapCell. Мне не известны
     * инструменты проверки этого на уровне аргумента функции, так что эта проверка
     * будет выполняться здесь.
     *
     * Для данной проверки создаётся один тестовый виджет, который после будет удалён.
     * С помощью dynamic_cast выясняется есть ли в древе наследовании переданного класса UMapCell */
    UUserWidget* TestCellWidget = CreateWidget<UUserWidget>(TileGridPanel, CellClass);
    if (!StaticCast<UMapCell*>(TestCellWidget)) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: CellClass was expected to inherit from UMapCell, but its class is %s"), *CellClass->GetName());

        //Если класс был некорректен, то тестовый виджет всё равно удаляется
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
    //Если проверка была пройдена, то тестовый виджет удаляется, он нужен был сугубо для проверки
    if (TestCellWidget) {
        TestCellWidget->RemoveFromParent();
    }

    if (MapDimensions.isValid) {
        //Чтобы не дёргать отрисовку лишний раз во время всего процесса забивания ячейками карты, следует сделать TileGridPanel колапсированной
        TileGridPanel->SetVisibility(ESlateVisibility::Collapsed);

        //Длина фрагмента карты, обусловленный размером таблиц, которые составляют карту
        int TableLength = MapDimensions.TableLength;
        //Длинна тайла карты, который необходим сугубо для оптимизации
        int MapTileLength = MapDimensions.MapTileLength;

        //Реальный размер полученной карты (в фрагментах)
        ColNum = MapDimensions.MaxCol - MapDimensions.MinCol + 1;
        RowNum = MapDimensions.MaxRow - MapDimensions.MinRow + 1;

        //Отображаемый размер карты, который никогда не превышает 3 фрагмента для лучшей производительности
        int DisplayedColNum = ColNum;
        int DisplayedRowNum = RowNum;

        if (DisplayedColNum > 3)
            DisplayedColNum = 3;
        if (DisplayedRowNum > 3)
            DisplayedRowNum = 3;

        //Размер отображаемой карты (в тайлах)
        int NumberOfMapTilesCols = DisplayedColNum * TableLength / MapTileLength;
        int NumberOfMapTilesRows = DisplayedRowNum * TableLength / MapTileLength;

        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: all checks have been passed, the TileGridPanel grid is ready to be filled with cells with dimensions: rows: %d, columns: %d"), NumberOfMapTilesRows, NumberOfMapTilesCols);

        //Само забиваение карты ячейками происходит в отдельном потоке
        AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [MapEditor, NumberOfMapTilesCols, NumberOfMapTilesRows, TableLength,
            MapTileLength, DisplayedColNum, DisplayedRowNum, TileGridPanel,  CellClass, MapTileClass, TilesCoordWrapper, this]() {

                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The TileGridPanel table population thread has been opened"));

                FVector2D TileSize(0, 0);

                /* Данный цикл и вложенный в него создают тайлы. При этом
                 * это происходит таким образом, чтобы самый первый тайл
                 * с наименьшим индексом был слева снизу, а последний с
                 * наибольшим - справа сверху.
                 * 
                 * Для этого строки перебираются обратным циклом - 
                 * от их количества (это число сразу переведено 
                 * в индекс вычитанием единицы) до нуля.
                 * Для столбцов же перебор обычный от нуля до их
                 * количества, при этом, из-за того, что здесь <,
                 * а не <=, преведение к индексу не требуется, так
                 * как последнее число всё равно усекается */
                for (int row = NumberOfMapTilesRows - 1; row >= 0; row--) {
                    for (int col = 0; col < NumberOfMapTilesCols; col++) {
                        //Карта состоит из тайлов, которые, в свою очередь, состоят из ячеек, так что сначала создаётся тайл
                        UMapTile* MapTile = StaticCast<UMapTile*>(CreateWidget<UUserWidget>(TileGridPanel, MapTileClass));

                        if (!MapTile) {
                            UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapTile was created with an error"));

                            //Виджет загрузки удалятся в основном потоке так как сделать это вне его невозможно
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

                        /* Тайл забивается ячейками по точно такому же принципу, что и карта тайлами -
                         * первая ячейка находится слева снизу, а последняя - справа сверху */
                        for (int tileRow = MapTileLength - 1; tileRow >= 0; tileRow--) {
                            for (int tileCol = 0; tileCol < MapTileLength; tileCol++) {
                                UMapCell* Cell = CreateWidget<UMapCell>(MapTile, CellClass);

                                if (!Cell) {
                                    UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: Cell was created with an error"));

                                    //Виджет загрузки удалятся в основном потоке так как сделать это вне его невозможно
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

                                //Добавление созданной ячейки в GridPanel производится в основном потоке так как сделать это вне его невозможно
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

                                /* Для дальнейшего выполнения кода требуется размер тайла.
                                 * И чтобы потом не запрашивать доступ к содержимому TileGridPanel,
                                 * здесь просто считывается этот размпер во время первой итерации */
                                if (tileRow == 0 && tileCol == MapTileLength - 1) {
                                    //Размер тайла срого равен общему размеру всех ячеек, никаких дополнительных отступов быть не должно
                                    TileSize = Cell->getSize() * MapTileLength;
                                }
                            }
                        }

                        //Добавление созданного тайла в GridPanel производится в основном потоке так как сделать это вне его невозможно
                        AsyncTask(ENamedThreads::GameThread, [TileGridPanel, MapTile, row, col, TilesCoordWrapper, this]() {
                            if (DetailedLogs)
                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Launched a GameThread call from the TileGridPanel table population thread to place a tile in a TileGridPanel table"));

                            TileGridPanel->AddChildToUniformGrid(MapTile, row, col);

                            if (DetailedLogs) {
                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The tile was placed in a TileGridPanel table at position col: %d row: %d"), row, col);
                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: A thread to place a tile in a TileGridPanel table has been closed"));
                            }
                        });

                        /* Созданные тайлы забиваются в координатную обёртку. При этом важен их индекс в порядке
                         * создания, чтобы, например, слева снизу был тайл 0x0, а справа сверху - 10x10.
                         * Для этого индекс обратного цикла по строкам разворачивается, чтобы передать
                         * инвертированное значение от того, что было уже инвертировано для корректного
                         * расположения тайлов в GridPanel */
                        TilesCoordWrapper->AddWidget(NumberOfMapTilesRows - 1 - row, col, MapTile);
                    }
                }

                //Чтобы таблицу не сжимало устанавливается минимальный размер слота
                TileGridPanel->SetMinDesiredSlotWidth(TileSize.X);
                TileGridPanel->SetMinDesiredSlotHeight(TileSize.Y);

                //Все взаимодействия, связанные с изменением состояний виджетов выполняются в основном потоке
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

                    //После полного забивания карты ячейками TileGridPanel возвращается сидимость
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

//Если не передать виджет загрузки, то загрузка будет будет без индикации
void UFillingMapWithCells::setLoadWidget(ULoadingWidget* newLoadingWidget)
{
    this->LoadingWidget = newLoadingWidget;

    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the setLoadWidget function: The download widget has been set"));
}

//Геттер количества фрагментов по горизонтали
int32 UFillingMapWithCells::GetColNum()
{
    return ColNum;
}

//Геттер количества фрагментов по вертикали
int32 UFillingMapWithCells::GetRowNum()
{
    return RowNum;
}
