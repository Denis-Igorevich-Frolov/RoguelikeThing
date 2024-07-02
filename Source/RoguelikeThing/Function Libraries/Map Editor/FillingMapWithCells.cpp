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
    //Получение GameInstance из мира
    GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (!GameInstance)
        UE_LOG(FillingMapWithCells, Warning, TEXT("Warning in FillingMapWithCells class in constructor - GameInstance was not retrieved from the world"));
}

/* Функция, заполняющая карту ячейками на основе БД.
 *
 * MapTileClass обязательно должен быть наследником
 * класса UMapTile или им самим, CellClass обязательно
 * должен быть наследником класса UMapCell или им самим */
FNumberOfTilesThatFit UFillingMapWithCells::FillMapEditorWithCells(FMapDimensions MapDimensions, UUniformGridPanel* TilesGridPanel, UClass* CellClass, UClass* MapTileClass,
    UTileBuffer* TileBuf, UMapEditor* MapEditor, UCoordWrapperOfTable* TilesCoordWrapper, UMapMatrix* MapMatrix, FVector2D WidgetAreaSize, float MaxDiffSizeFromScalingToLargerSide)
{
    //Координатная обёртка изначально должна быть полность пустой во время заполнения карты
    TilesCoordWrapper->Clear();

    //Проверка корректности указателей
    if (!TilesGridPanel) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: TilesGridPanel is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if(GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return FNumberOfTilesThatFit();
    }
    if (!CellClass) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: CellClass is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return FNumberOfTilesThatFit();
    }
    if (!MapTileClass) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapTileClass is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return FNumberOfTilesThatFit();
    }
    if (!MapEditor) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapEditor is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return FNumberOfTilesThatFit();
    }
    if (!TilesCoordWrapper) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: TilesCoordWrapper is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return FNumberOfTilesThatFit();
    }
    if (!MapMatrix) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapMatrix is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return FNumberOfTilesThatFit();
    }

    /* Здесь проверяется корректность переданного класса, по которому будут делаться тайлы.
     * Переданный MapTileClass имеет тип данных UClass*, но для корректной работы кода
     * этот класс обязательно должен наследоваться от UMapTile. Мне не известны
     * инструменты проверки этого на уровне аргумента функции, так что эта проверка
     * будет выполняться здесь.
     *
     * Для данной проверки создаётся один тестовый виджет, который после будет удалён.
     * С помощью dynamic_cast выясняется есть ли в древе наследовании переданного класса UMapTile */
    UUserWidget* TestGridWidget = CreateWidget<UUserWidget>(TilesGridPanel, MapTileClass);
    if (!dynamic_cast<UMapTile*>(TestGridWidget)) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapFragmentClass was expected to inherit from UMapTile, but its class is %s"), *MapTileClass->GetName());

        //Если класс был некорректен, то тестовый виджет всё равно удаляется
        if (TestGridWidget) {
            TestGridWidget->RemoveFromParent();
        }

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return FNumberOfTilesThatFit();
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
    UUserWidget* TestCellWidget = CreateWidget<UUserWidget>(TilesGridPanel, CellClass);
    if (!StaticCast<UMapCell*>(TestCellWidget)) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: CellClass was expected to inherit from UMapCell, but its class is %s"), *CellClass->GetName());

        //Если класс был некорректен, то тестовый виджет всё равно удаляется
        if (TestCellWidget) {
            TestCellWidget->RemoveFromParent();
        }

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return FNumberOfTilesThatFit();
    }
    /* Тестовый виджет ячейки пока не удаляется, он ещё пригодится для вычисления размеров тайла. Вычислять будем именно с тестовой
     * ячейки потому что здесь уже проведена точная и недвусмысленная проверка, что ячейка - это ячейка и она определённо существует,
     * в то время как ячейки, созданные в процессе заполнения карты могут быть не настолько корректными, например может быть загружена
     * полностью пустая карта вовсе без ячеек, да и банально до их указателя добираться дольше и сложнее через несколько StaticCast'ов. */

    if (MapDimensions.isValid) {
        //Чтобы не дёргать отрисовку лишний раз во время всего процесса забивания ячейками карты, следует сделать TilesGridPanel колапсированной
        TilesGridPanel->SetVisibility(ESlateVisibility::Collapsed);

        int TableLength = MapDimensions.TableLength;
        int MapTileLength = MapDimensions.MapTileLength;

        //Реальный размер полученной карты (в фрагментах)
        ColsNum = MapDimensions.MaxCol - MapDimensions.MinCol + 1;
        RowsNum = MapDimensions.MaxRow - MapDimensions.MinRow + 1;

        int NumberOfMapTilesCols = ColsNum * (TableLength / MapTileLength);
        int NumberOfMapTilesRows = RowsNum * (TableLength / MapTileLength);

        if (TestCellWidget) {
            FVector2D CellSize = static_cast<UMapCell*>(TestCellWidget)->getSize();
            TileSize = CellSize * MapTileLength;
        }
        //И только сейчас удаляется тестовый виджет ячейки
        if (TestCellWidget) {
            TestCellWidget->RemoveFromParent();
        }

        //Количество чанков, влезающих на экран
        NumberOfTilesColsThatFitOnScreen = (WidgetAreaSize.X) / TileSize.X + 1;
        NumberOfTilesRowsThatFitOnScreen = (WidgetAreaSize.Y) / TileSize.Y + 1;

        FVector2D MaxDiffSizeFromScaling(0, 0);
        if (NumberOfTilesColsThatFitOnScreen > NumberOfTilesRowsThatFitOnScreen)
            MaxDiffSizeFromScaling = 
            FVector2D(MaxDiffSizeFromScalingToLargerSide, MaxDiffSizeFromScalingToLargerSide * ((float)NumberOfTilesRowsThatFitOnScreen / NumberOfTilesColsThatFitOnScreen));
        else
            MaxDiffSizeFromScaling = 
            FVector2D(MaxDiffSizeFromScalingToLargerSide * ((float)NumberOfTilesColsThatFitOnScreen / NumberOfTilesRowsThatFitOnScreen), MaxDiffSizeFromScalingToLargerSide);

        NumberOfTilesColsThatFitOnScreen += ceil((MaxDiffSizeFromScaling.X) / TileSize.X);
        NumberOfTilesRowsThatFitOnScreen += ceil((MaxDiffSizeFromScaling.Y) / TileSize.Y);

        //Если количество чанков, влезающих на экран, больше фактического количества чанков, то оно усекается
        if (NumberOfTilesColsThatFitOnScreen > NumberOfMapTilesCols)
            NumberOfTilesColsThatFitOnScreen = NumberOfMapTilesCols;
        if (NumberOfTilesRowsThatFitOnScreen > NumberOfMapTilesRows)
            NumberOfTilesRowsThatFitOnScreen = NumberOfMapTilesRows;

        //Переманные, отражающие чётность фактического количества чанков
        bool IsNumberOfTilesColsEven = (NumberOfMapTilesCols % 2) == 0;
        bool IsNumberOfTilesRowsEven = (NumberOfMapTilesRows % 2) == 0;

        //Переменные, отражающие чётность влезающего на экран количества чанков
        bool IsNumberOfFittingTilesColsEven = (NumberOfTilesColsThatFitOnScreen % 2) == 0;
        bool IsNumberOfFittingTilesRowsEven = (NumberOfTilesRowsThatFitOnScreen % 2) == 0;

        /* Чётность фактического и влезающего на экран количества чанков всегда должна совпадать для нормальной
         * центровки таблицы. При расхождении чётности, количество влезающих на кран ячеек увеличивается на 1.
         * Так как выше уже была проведена усекающая проверка, не дающая влезающему количеству ячеек превысить
         * фактическое, а быть равными эти количества при расходящейся чётности также не могут, то при увеличении
         * влезающего количества ячеек на 1, превышение фактического количества никогда не произоёдёт. */
        if (IsNumberOfTilesColsEven != IsNumberOfFittingTilesColsEven)
            NumberOfTilesColsThatFitOnScreen++;
        if (IsNumberOfTilesRowsEven != IsNumberOfFittingTilesRowsEven)
            NumberOfTilesRowsThatFitOnScreen++;

        int StartingPositionRow = (NumberOfMapTilesRows - NumberOfTilesRowsThatFitOnScreen) / 2;
        int StartingPositionCol = (NumberOfMapTilesCols - NumberOfTilesColsThatFitOnScreen) / 2;

        if (GameInstance && GameInstance->LogType != ELogType::NONE)
            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: all checks have been passed, the TilesGridPanel grid is ready to be filled with cells with dimensions: rows: %d, columns: %d"), NumberOfMapTilesRows, NumberOfMapTilesCols);

        //Само забиваение карты ячейками происходит в отдельном потоке
        AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [MapEditor, TableLength, TestCellWidget, MapTileLength, TilesGridPanel, TileBuf,
            CellClass, MapTileClass, TilesCoordWrapper, MapMatrix, StartingPositionRow, StartingPositionCol, NumberOfMapTilesRows, NumberOfMapTilesCols, this]() {

                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The TilesGridPanel table population thread has been opened"));

                UMapTile* LastMapTile = nullptr;

                TileBuf->Clear();
                if(!TileBuf->ScoreToMaximum())
                    UE_LOG(FillingMapWithCells, Log, TEXT("fffffffffffff"));

                MinOriginalVisibleTile = FGridCoord(StartingPositionRow, StartingPositionCol);
                MaxOriginalVisibleTile = FGridCoord(StartingPositionRow + (NumberOfTilesRowsThatFitOnScreen - 1), StartingPositionCol + (NumberOfTilesColsThatFitOnScreen - 1));

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
                for (int row = StartingPositionRow + NumberOfTilesRowsThatFitOnScreen - 1; row >= StartingPositionRow; row--) {
                    for (int col = StartingPositionCol; col < NumberOfTilesColsThatFitOnScreen + StartingPositionCol; col++) {
                        //Карта состоит из тайлов, которые, в свою очередь, состоят из ячеек, так что сначала создаётся тайл
                        UMapTile* MapTile = StaticCast<UMapTile*>(CreateWidget<UUserWidget>(TilesGridPanel, MapTileClass));

                        if (!MapTile) {
                            UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapTile was created with an error"));

                            //Виджет загрузки удалятся в основном потоке так как сделать это вне его невозможно
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

                                return FNumberOfTilesThatFit();
                                });
                        }

                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: An uninitialized MapTile is created with coordinates row: %d col: %d"), row, col);

                        if (!MapTile->FillingWithCells(MapTileLength, CellClass, MapEditor)) {
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

                                return FNumberOfTilesThatFit();
                                });
                        }

                        ///* Тайл забивается ячейками по точно такому же принципу, что и карта тайлами -
                        // * первая ячейка находится слева снизу, а последняя - справа сверху */
                        //for (int tileRow = MapTileLength - 1; tileRow >= 0; tileRow--) {
                        //    for (int tileCol = 0; tileCol < MapTileLength; tileCol++) {
                        //        UMapCell* Cell = CreateWidget<UMapCell>(MapTile, CellClass);

                        //        if (!Cell) {
                        //            UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: Cell was created with an error"));

                        //            //Виджет загрузки удалятся в основном потоке так как сделать это вне его невозможно
                        //            AsyncTask(ENamedThreads::GameThread, [MapEditor, TilesGridPanel, this]() {
                        //                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                        //                    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Launched a GameThread call from the TilesGridPanel table population thread to remove the loading widget"));

                        //                if (LoadingWidget) {
                        //                    LoadingWidget->LoadingComplete(false);
                        //                    LoadingWidget->RemoveFromParent();

                        //                    if (GameInstance && GameInstance->LogType != ELogType::NONE)
                        //                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
                        //                }
                        //                else
                        //                    if (GameInstance && GameInstance->LogType != ELogType::NONE)
                        //                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: There was no download widget"));

                        //                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                        //                    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: A thread to remove the loading widget has been closed"));

                        //                return FNumberOfTilesThatFit();
                        //                });
                        //        }

                        //        //Ячейке передаётся её глобальная координата, равная её порядковому номеру по вертикали и горизонтали (точка отсчёта с 1)
                        //        Cell->MyCoord = FCellCoord(row * MapTileLength + tileRow + 1, col * MapTileLength + tileCol + 1);
                        //        //Также ей передаётся указатель на её редактор карт
                        //        Cell->MyEditor = MapEditor;

                        //        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                        //            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: An uninitialized cell is created with coordinates row: %d col: %d for a MapTile with coordinates row: %d col: %d"), tileRow, tileCol, row, col);

                        //        //Добавление созданной ячейки в GridPanel производится в основном потоке так как сделать это вне его невозможно
                        //        AsyncTask(ENamedThreads::GameThread, [MapTile, Cell, tileRow, tileCol, row, col, this]() {
                        //            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                        //                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Launched a GameThread call from the TilesGridPanel table population thread to place a cell in a tile"));

                        //            if (MapTile->GetGridPanel()) {
                        //                MapTile->GetGridPanel()->AddChildToUniformGrid(Cell, tileRow, tileCol);

                        //                if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                        //                    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The cell was placed in a tile located at coordinates col: %d row: %d, at position col: %d row: %d"), tileRow, tileCol, row, col);
                        //            }
                        //            else
                        //                UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: Failed to get GridPanel from MapTile"));

                        //            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                        //                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: A thread to place a cell in a tile has been closed"));
                        //            });

                        //        //Теперь пришло время придать новой ячейке необходимый стиль. Для этого из БД читается тип структуры ячейки
                        //        FMapEditorBrushType CellType = MapMatrix->GetValueOfMapChunkStructureCellByGlobalIndex(row * MapTileLength + tileRow + 1, col * MapTileLength + tileCol + 1, false);

                        //        /* И затем в соответствии с полученым типом, ячейке присваивается необходимый стиль.
                        //         * При этом пустой стиль назначать не надо, он и так является стилем по умолчанию */
                        //        switch (CellType)
                        //        {
                        //        case FMapEditorBrushType::Corridor:
                        //            Cell->SetCorridorStyle(MapMatrix->CheckNeighbourhoodOfCell(MatrixType::ChunkStructure, row * MapTileLength + tileRow + 1, col * MapTileLength + tileCol + 1, false));
                        //            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                        //                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Cell Row: %d Col: %d is set to the corridor style"), row * MapTileLength + tileRow + 1, col * MapTileLength + tileCol + 1);
                        //            break;
                        //        case FMapEditorBrushType::Room:
                        //            Cell->SetRoomStyle();
                        //            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                        //                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Cell Row: %d Col: %d is set to the room style"), row * MapTileLength + tileRow + 1, col * MapTileLength + tileCol + 1);
                        //            break;
                        //        default:
                        //            break;
                        //        }

                        //        /* Созданные ячейки забиваются в координатную обёртку. При этом важен их индекс в порядке
                        //         * создания, чтобы, например, слева снизу была ячейка 0x0, а справа сверху - 5x5.
                        //         * Для этого индекс обратного цикла по строкам разворачивается, чтобы передать
                        //         * инвертированное значение от того, что было уже инвертировано для корректного
                        //         * расположения ячеек в GridPanel */
                        //        MapTile->CellsCoordWrapper->AddWidget(MapTileLength - 1 - tileRow, tileCol, Cell);
                        //    }
                        //}

                        //Добавление созданного тайла в GridPanel производится в основном потоке так как сделать это вне его невозможно
                        AsyncTask(ENamedThreads::GameThread, [TilesGridPanel, MapTile, row, col, TilesCoordWrapper, this]() {
                            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Launched a GameThread call from the TilesGridPanel table population thread to place a tile in a TilesGridPanel table"));

                            TilesGridPanel->AddChildToUniformGrid(MapTile, row, col);

                            if (GameInstance && GameInstance->LogType == ELogType::DETAILED) {
                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The tile was placed in a TilesGridPanel table at position col: %d row: %d"), row, col);
                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: A thread to place a tile in a TilesGridPanel table has been closed"));
                            }
                        });

                        /* Созданные тайлы забиваются в координатную обёртку. При этом важен их индекс в порядке
                         * создания, чтобы, например, слева снизу был тайл 0x0, а справа сверху - 10x10.
                         * Для этого индекс обратного цикла по строкам разворачивается, чтобы передать
                         * инвертированное значение от того, что было уже инвертировано для корректного
                         * расположения тайлов в GridPanel */
                        TilesCoordWrapper->AddWidget(NumberOfMapTilesRows - row - 1, col, MapTile);

                        LastMapTile = MapTile;
                    }
                }

                UMapTile* MinTile = StaticCast<UMapTile*>(CreateWidget<UUserWidget>(TilesGridPanel, MapTileClass));
                //TilesCoordWrapper->AddWidget(NumberOfMapTilesRows - 1, 0, MinTile);

                UMapTile* MaxTile = StaticCast<UMapTile*>(CreateWidget<UUserWidget>(TilesGridPanel, MapTileClass));
                //TilesCoordWrapper->AddWidget(0, NumberOfMapTilesCols - 1, MinTile);

                AsyncTask(ENamedThreads::GameThread, [TilesGridPanel, MinTile, MaxTile, NumberOfMapTilesRows, NumberOfMapTilesCols, this]() {
                    TilesGridPanel->AddChildToUniformGrid(MinTile, NumberOfMapTilesRows - 1, 0);
                    TilesGridPanel->AddChildToUniformGrid(MaxTile, 0, NumberOfMapTilesCols - 1);
                });

                //Чтобы таблицу не сжимало устанавливается минимальный размер слота
                TilesGridPanel->SetMinDesiredSlotWidth(TileSize.X);
                TilesGridPanel->SetMinDesiredSlotHeight(TileSize.Y);

                //Все взаимодействия, связанные с изменением состояний виджетов выполняются в основном потоке
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

                    //После полного забивания карты ячейками TilesGridPanel возвращается сидимость
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

        return FNumberOfTilesThatFit();
    }

    return FNumberOfTilesThatFit(NumberOfTilesColsThatFitOnScreen, NumberOfTilesRowsThatFitOnScreen);
}

//Если не передать виджет загрузки, то загрузка будет будет без индикации
void UFillingMapWithCells::setLoadWidget(ULoadingWidget* newLoadingWidget)
{
    this->LoadingWidget = newLoadingWidget;

    if (GameInstance && GameInstance->LogType != ELogType::NONE)
        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the setLoadWidget function: The download widget has been set"));
}

//Геттер количества фрагментов по горизонтали
int32 UFillingMapWithCells::GetColNum()
{
    return ColsNum;
}

//Геттер количества фрагментов по вертикали
int32 UFillingMapWithCells::GetRowNum()
{
    return RowsNum;
}

FNumberOfTilesThatFit::FNumberOfTilesThatFit(int Cols, int Rows) : Cols(Cols), Rows(Rows)
{}
