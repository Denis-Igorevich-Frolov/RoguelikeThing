// Fill out your copyright notice in the Description page of Project Settings.


#include "RoguelikeThing/Function Libraries/Map Editor/FillingMapWithCells.h"
#include "RoguelikeThing/Widgets/LoadingWidget.h"
#include "RoguelikeThing/Widgets/MapEditor/MapTile.h"
#include "RoguelikeThing/Widgets/MapEditor/MapCell.h"
#include "RoguelikeThing/Widgets/MapEditor/MapEditor.h"
#include "RoguelikeThing/Interfaces/ContainingTileTableInterface.h"
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
void UFillingMapWithCells::FillMapEditorWithCells(FMapDimensions MapDimensions, UUniformGridPanel* TilesGridPanel,
    UClass* CellClass, UClass* MapTileClass, UTileBuffer* TileBuf, UObject* MapContainer, UCoordWrapperOfTable* TilesCoordWrapper,
    UMapMatrix* Map, FVector2D WidgetAreaSize, UMySaveGame* SaveGame, bool FillOnlyNonEmptyArea)
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
    if (!MapContainer) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapContainer is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return;
    }
    if (!MapContainer->GetClass()->ImplementsInterface(UContainingTileTableInterface::StaticClass())) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapContainer is not implementing ContainingTileTableInterface"));

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
    if (!Map) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapMatrix is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return;
    }
    if (!TileBuf) {
        UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: TileBuf is a null pointer"));

        if (LoadingWidget) {
            LoadingWidget->LoadingComplete(false);
            LoadingWidget->RemoveFromParent();

            if (GameInstance && GameInstance->LogType != ELogType::NONE)
                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
        }

        return;
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

        return;
    }
    //Если проверка была пройдена, то тестовый виджет удаляется, он нужен был сугубо для проверки
    if (TestGridWidget) {
        if (TestGridWidget->IsValidLowLevel()) {
            TestGridWidget->ConditionalBeginDestroy();
            TestGridWidget->MarkPendingKill();
        }
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

        return;
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
        int NumberOfTilesInChunc = TableLength / MapTileLength;

        if (TestCellWidget) {
            FVector2D CellSize = static_cast<UMapCell*>(TestCellWidget)->getSize();
            TileSize = CellSize * MapTileLength;
        }
        //И только сейчас удаляется тестовый виджет ячейки
        if (TestCellWidget) {
            if (TestCellWidget->IsValidLowLevel()) {
                TestCellWidget->ConditionalBeginDestroy();
                TestCellWidget->MarkPendingKill();
            }
            TestCellWidget->RemoveFromParent();
        }

        //Количество чанков, влезающих на экран
        NumberOfTilesColsThatFitOnScreen = ceil((WidgetAreaSize.X) / TileSize.X);
        NumberOfTilesRowsThatFitOnScreen = ceil((WidgetAreaSize.Y) / TileSize.Y);

        if (GameInstance && GameInstance->LogType != ELogType::NONE)
            UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: all checks have been passed, the TilesGridPanel grid is ready to be filled with cells"));

        //Все старые значения очищаются из буфера тайлов и заполняются новыми
        TileBuf->Clear();
        if (!TileBuf->FillToMaximum()) {
            UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapMatrix is a null pointer"));
            if (LoadingWidget) {
                LoadingWidget->LoadingComplete(false);
                LoadingWidget->RemoveFromParent();

                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The download widget has been removed"));
            }

            return;
        }

        //Само забиваение карты ячейками происходит в отдельном потоке
        AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [MapContainer, TableLength, MapTileLength,TilesGridPanel, CellClass, MapTileClass, TilesCoordWrapper,
            Map, MapDimensions, NumberOfTilesInChunc, FillOnlyNonEmptyArea, SaveGame, this]() {

                if (GameInstance && GameInstance->LogType != ELogType::NONE)
                    UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The TilesGridPanel table population thread has been opened"));

                //Все непустые ячейки предзагружаются из БД
                Map->FillTerrainOfTiles(SaveGame);

                FGridCoord StartingMinTileCoord = FGridCoord(0, 0);

                //Если заполнение таблицы будет происходить только в габаритах непустых ячеек, то здесь считываются минимальные непустые координаты матрицы
                if (FillOnlyNonEmptyArea) {
                    StartingMinTileCoord = FGridCoord(Map->GetMinNoEmptyTileCoord().Row, Map->GetMinNoEmptyTileCoord().Col);
                }

                //Реальный размер полученной карты (в фрагментах)
                ColsNum = 0;
                RowsNum = 0;

                int NumberOfMapTilesCols = 0;
                int NumberOfMapTilesRows = 0;

                if (FillOnlyNonEmptyArea) {
                    NumberOfMapTilesCols = Map->GetMaxNoEmptyTileCoord().Col + 1 - Map->GetMinNoEmptyTileCoord().Col;
                    NumberOfMapTilesRows = Map->GetMaxNoEmptyTileCoord().Row + 1 - Map->GetMinNoEmptyTileCoord().Row;

                    //Реальный размер полученной карты (в фрагментах)
                    ColsNum = NumberOfMapTilesCols / NumberOfTilesInChunc;
                    RowsNum = NumberOfMapTilesRows / NumberOfTilesInChunc;
                }
                else {
                    //Реальный размер полученной карты (в фрагментах)
                    ColsNum = MapDimensions.MaxCol - MapDimensions.MinCol + 1;
                    RowsNum = MapDimensions.MaxRow - MapDimensions.MinRow + 1;

                    NumberOfMapTilesCols = ColsNum * NumberOfTilesInChunc;
                    NumberOfMapTilesRows = RowsNum * NumberOfTilesInChunc;
                }

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
                if ((IsNumberOfTilesColsEven != IsNumberOfFittingTilesColsEven) && (NumberOfTilesColsThatFitOnScreen + 1 <= NumberOfMapTilesCols))
                    NumberOfTilesColsThatFitOnScreen++;
                if ((IsNumberOfTilesRowsEven != IsNumberOfFittingTilesRowsEven) && (NumberOfTilesRowsThatFitOnScreen + 1 <= NumberOfMapTilesRows))
                    NumberOfTilesRowsThatFitOnScreen++;

                int StartingPositionRow = (NumberOfMapTilesRows - NumberOfTilesRowsThatFitOnScreen) / 2;
                int StartingPositionCol = (NumberOfMapTilesCols - NumberOfTilesColsThatFitOnScreen) / 2;

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
                        UPROPERTY()
                        UMapTile* NewTile = CreateWidget<UMapTile>(TilesGridPanel, MapTileClass);

                        if (!NewTile) {
                            UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: MapTile was created with an error"));

                            //Виджет загрузки удалятся в основном потоке так как сделать это вне его невозможно
                            AsyncTask(ENamedThreads::GameThread, [MapContainer, TilesGridPanel, this]() {
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

                        NewTile->SetMyCoord(FCellCoord(row + StartingMinTileCoord.Row, col + StartingMinTileCoord.Col));
                        //Из матрицы карты берётся предзагруженный ландшафт тайла
                        NewTile->SetMyTerrainOfTile(Map->GetTerrainOfTile(NewTile->GetMyCoord()));

                        //Тайл заполнаяется ячейками по переданному классу
                        if (!NewTile->FillingWithCells(MapTileLength, CellClass, MapContainer, Map)) {
                            AsyncTask(ENamedThreads::GameThread, [MapContainer, TilesGridPanel, this]() {
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

                        //Добавление созданного тайла в GridPanel производится в основном потоке так как сделать это вне его невозможно
                        AsyncTask(ENamedThreads::GameThread, [TilesGridPanel, NewTile, row, col, TilesCoordWrapper, NumberOfMapTilesRows, this]() {
                            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Launched a GameThread call from the TilesGridPanel table population thread to place a tile in a TilesGridPanel table"));

                            UPROPERTY()
                            UUniformGridSlot* GridSlot = TilesGridPanel->AddChildToUniformGrid(NewTile, row, col);

                            if (GameInstance && GameInstance->LogType == ELogType::DETAILED) {
                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The tile was placed in a TilesGridPanel table at position col: %d row: %d"), row, col);
                                UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: A thread to place a tile in a TilesGridPanel table has been closed"));
                            }

                            /* Созданные тайлы забиваются в координатную обёртку. При этом важен их индекс в порядке
                             * создания, чтобы, например, слева снизу был тайл 0x0, а справа сверху - 10x10.
                             * Для этого индекс обратного цикла по строкам разворачивается, чтобы передать
                             * инвертированное значение от того, что было уже инвертировано для корректного
                             * расположения тайлов в GridPanel */
                            TilesCoordWrapper->AddWidget(NumberOfMapTilesRows - row - 1, col, NewTile, GridSlot);
                        });
                    }
                }

                /* Создаётся пустой тайл, который помещается в наибольший слот таблицы карты, растягивая её до
                 * таких габаритов, которые были бы, если все тайлы карты единовременно находились бы на ней */
                UPROPERTY()
                UMapTile* StretcherTile = CreateWidget<UMapTile>(TilesGridPanel, MapTileClass);
                AsyncTask(ENamedThreads::GameThread, [TilesGridPanel, StretcherTile, NumberOfMapTilesRows, NumberOfMapTilesCols, this]() {
                    TilesGridPanel->AddChildToUniformGrid(StretcherTile, NumberOfMapTilesRows - 1, NumberOfMapTilesCols - 1);
                });

                //Чтобы таблицу не сжимало устанавливается минимальный размер слота
                TilesGridPanel->SetMinDesiredSlotWidth(TileSize.X);
                TilesGridPanel->SetMinDesiredSlotHeight(TileSize.Y);

                //Все взаимодействия, связанные с изменением состояний виджетов выполняются в основном потоке
                AsyncTask(ENamedThreads::GameThread, [MapContainer, TilesGridPanel, this]() {
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

                    //Вызывается эвент контейнера таблицы, говорящий о том, что её контент обновился
                    Cast<IContainingTileTableInterface>(MapContainer)->Execute_UpdateItemAreaContent(MapContainer);

                    //После полного забивания карты ячейками TilesGridPanel возвращается видимость
                    TilesGridPanel->SetVisibility(ESlateVisibility::Visible);
                    if (GameInstance && GameInstance->LogType != ELogType::NONE)
                        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: A thread to configure the necessary widgets at the end of filling the TilesGridPanel table and removing the loading widget has been closed"));
                });

                //Так как старая карта, при условии что она была, удалялась, то следует форсировать сборку мусора
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

//Если не передать виджет загрузки, то загрузка будет будет без индикации
void UFillingMapWithCells::setLoadWidget(ULoadingWidget* newLoadingWidget)
{
    this->LoadingWidget = newLoadingWidget;

    if (GameInstance && GameInstance->LogType != ELogType::NONE)
        UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the setLoadWidget function: The download widget has been set"));
}

bool UFillingMapWithCells::LoadWidgetIsValide()
{
    return (bool)LoadingWidget && LoadingWidget->IsInViewport();
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
