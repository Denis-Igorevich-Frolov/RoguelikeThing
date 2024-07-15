// Denis Igorevich Frolov did all this. Once there. All things reserved.

#include "RoguelikeThing/Function Libraries/TileTablesOptimizationTools.h"
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(TileTablesOptimizationTools);

UTileTablesOptimizationTools::UTileTablesOptimizationTools()
{
    //Получение GameInstance из мира
    GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (!GameInstance)
        UE_LOG(TileTablesOptimizationTools, Warning, TEXT("Warning in TileTablesOptimizationTools class in constructor - GameInstance was not retrieved from the world"));
}

void UTileTablesOptimizationTools::AsynchronousAreaFilling(FGridDimensions AreaDimensions, int NumberOfMapTilesRows, FString ss)
{
    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [ss, AreaDimensions, NumberOfMapTilesRows, this]() {
        for (int col = AreaDimensions.Min.Col; col <= AreaDimensions.Max.Col; col++) {
            for (int row = AreaDimensions.Min.Row; row <= AreaDimensions.Max.Row; row++) {
                AsyncTask(ENamedThreads::GameThread, [ss, AreaDimensions, col, row, NumberOfMapTilesRows, this]() {
                    if (TilesCoordWrapper->FindWidget(row, col)) {
                        UE_LOG(TileTablesOptimizationTools, Warning, TEXT("%s add fffffff r: %d, c: %d"), *ss, row, col);
                        //continue;
                    }
                    //else
                        //UE_LOG(TileTablesOptimizationTools, Log, TEXT("%s add ddddddd r: %d, c: %d"), *ss, row, col);

                    UAbstractTile* Tile = TilesBuf->GetTile();

                    if (Tile) {
                        Tile->SetMyCoord(FCellCoord((NumberOfMapTilesRows - row) - 1, col));
                        UUniformGridSlot* GridSlot = TilesGridPanel->AddChildToUniformGrid(Tile, (NumberOfMapTilesRows - row) - 1, col);
                        TilesCoordWrapper->AddWidget(row, col, Tile, GridSlot);
                        Tile->OnAddedEvent(MapTileLength, MapMatrix);
                    }
                    else
                        UE_LOG(TileTablesOptimizationTools, Warning, TEXT("AAAAAAAAAAAAA"));
                    });
            }
        }
    });
}

void UTileTablesOptimizationTools::AsynchronousAreaRemoving(FGridDimensions AreaDimensions, int NumberOfMapTilesRows, FString ss)
{
    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [ss, AreaDimensions, NumberOfMapTilesRows, this]() {
        GetWorld()->ForceGarbageCollection(true);
        for (int col = AreaDimensions.Min.Col; col <= AreaDimensions.Max.Col; col++) {
                for (int row = AreaDimensions.Min.Row; row <= AreaDimensions.Max.Row; row++) {
                    AsyncTask(ENamedThreads::GameThread, [ss, col, row, AreaDimensions, this]() {
                        if (TilesBuf->BufSize() + 1 > TilesBuf->GetMaxSize()) {
                            if (!TilesCoordWrapper->RemoveWidget(row, col)) {
                                UE_LOG(TileTablesOptimizationTools, Warning, TEXT("%s remove fffffff r: %d, c: %d"), *ss, row, col);
                            }
                            //else
                                //UE_LOG(TileTablesOptimizationTools, Log, TEXT("%s remove ddddddd r: %d, c: %d"), *ss, row, col);
                        }
                        else {
                            UUniformGridSlot* GridSlot = TilesCoordWrapper->FindGridSlot(row, col);
                            if (GridSlot) {
                                GridSlot->SetColumn(-1);
                                GridSlot->SetRow(-1);
                            }
                            else
                                UE_LOG(TileTablesOptimizationTools, Warning, TEXT("%s remove 2 ssdsdfsdf r: %d, c: %d"), *ss, row, col);

                            UAbstractTile* Tile = TilesCoordWrapper->FindWidget(row, col);
                            Tile->ClearFilledCells();

                            if(!TilesCoordWrapper->RemoveCoord(row, col)) {
                                UE_LOG(TileTablesOptimizationTools, Warning, TEXT("%s remove 2 fffffff r: %d, c: %d"), *ss, row, col);
                            }
                            //else
                                //UE_LOG(TileTablesOptimizationTools, Log, TEXT("%s remove 2 ddddddd r: %d, c: %d"), *ss, row, col);

                            TilesBuf->AddTile(Tile);
                        }
                        });
                }
        }
        GetWorld()->ForceGarbageCollection(false);
    });
}

void UTileTablesOptimizationTools::Init(UUniformGridPanel* refTilesGridPanel, UCoordWrapperOfTable* refTilesCoordWrapper, UTileBuffer* refTilesBuf, UMapMatrix* refMapMatrix,
    FGridDimensions originalDimensions, FVector2D widgetAreaSize, FVector2D tileSize, FVector2D minContentSize, int numberOfTileRowsInTable, int numberOfTileColsInTable)
{
    TilesGridPanel = refTilesGridPanel;
    TilesCoordWrapper = refTilesCoordWrapper;
    TilesBuf = refTilesBuf;
    MapMatrix = refMapMatrix;

    this->OriginalDimensions = originalDimensions;
    this->TileSize = tileSize;
    this->MinContentSize = minContentSize;
    this->NumberOfTileRowsInTable = numberOfTileRowsInTable;
    this->NumberOfTileColsInTable = numberOfTileColsInTable;
    this->WidgetAreaSize = widgetAreaSize;
    this->MapTileLength = MapMatrix->GetMapTileLength();
    OldDimensions = OriginalDimensions;
    CurrentDimensions = OriginalDimensions;

    FGridCoord MaxCoord = OriginalDimensions.Max;
    FGridCoord MinCoord = OriginalDimensions.Min;
    OriginalDimensionsSize = FVector2D((MaxCoord.Col - MinCoord.Col) * TileSize.X, (MaxCoord.Row - MinCoord.Row) * TileSize.Y);

    MaximumContentSize = FVector2D(NumberOfTileColsInTable * TileSize.X, NumberOfTileRowsInTable * TileSize.Y);
    SizeDifference = MaximumContentSize - WidgetAreaSize;

    if (MaximumContentSize.X < MinContentSize.X)
        SizeDifference.X = 0;
    if (MaximumContentSize.Y < MinContentSize.Y)
        SizeDifference.Y = 0;

    /* Расстояние, при преодолении которого появляется первый новый тайл вычисляется путём нахождения половины разницы
     * между размерами изначально видимой области и области отображаемого контента. Тем самым находинся вектор, равный
     * такому размеру, который как бы торчит за пределами виджета, внутрь которого помещена тайловая таблица */
    DistanceToAppearanceOfFirstNewTile = (OriginalDimensionsSize - WidgetAreaSize) / 2.0;

    IsInit = true;
}

//Функция, изменяющая видимость тайлов от сдвига или масштабирования таблицы
void UTileTablesOptimizationTools::ChangingVisibilityOfTableTiles(FVector2D Bias, float ZoomMultiplier, int NumberOfMapTilesRows)
{
    if (!IsInit) {
        return;
    }

    if (!TilesCoordWrapper) {
        UE_LOG(TileTablesOptimizationTools, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: TilesCoordWrapper is not valid"));
        return;
    }

    /* При перетаскивании контента сдвиг мыши представляет собой разницу между изначальной позицией курсора
     * в момент нажатия и его текущей позицией, что делает разницу по координате X при перетаскивании справа
     * налево отрицательной. Оно и логично, но мне легче воспринимать не сдвиг контента в своей области налево,
     * а как бы "сдвиг камеры" по всей площади контента вправо, поэтому координата разворачивается */
    Bias.X *= -1;

    FVector2D CurrentSizeDifference = MaximumContentSize - (WidgetAreaSize / ZoomMultiplier);

    //Коорддинаты тайлов, отражающие сдвиг контента, которые будут прибавлены к изначальным координатам для вычисления конечной позиции контента
    FGridCoord MinBiasCoord;
    FGridCoord MaxBiasCoord;

    //Смещение по X больше 0 означает сдвиг видимой области вправо
    if (Bias.X > 0) {
        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The visible area has moved to the right of the center"));

        //Если камера не уткнулась в край контента, то все изменения происходят в обычном режиме
        if (Bias.X < CurrentSizeDifference.X / 2.0 - TileSize.X) {
            /* Смещение минимальной, а конкретнее левой, координаты при сдвиге камеры слева направо всегда отвечает за погашение лишних левых ячеек.
             * Расстояние до появления первого нового тайла здесь прибавляется к ширине смещения, чтобы учесть, что первый тайл можно погасить раньше
             * остальных, а именно в момент набора суммы смещения и расстояния до появления нового тайла равной ширине хотя бы 1 целого тайла. То есть
             * смещению достаточно будет покрыть расстояние, равное разнице между шириной тайла и расстоянием до появления первого тайла, чтобы сокрыть
             * слева этот первый тайл */
            MinBiasCoord.Col = (Bias.X + DistanceToAppearanceOfFirstNewTile.X) / TileSize.X;
            /* Смещение максимальной, а конкретнее правой, координаты при сдвиге камеры слева направо всегда отвечает за отображение новых правых ячеек.
             * Разница ширины тайла и расстояния до появления первого нового тайла здесь прибавляется к ширине смещения, чтобы учесть, что первый тайл
             * можно отобразить раньше остальных, а именно в момент набора суммы смещения и остатка от ширины тайла равной ширине хотя бы 1 целого тайла.
             * То есть смещению достаточно будет покрыть расстояние, равное расстоянию до появления первого тайла, чтобы отобразить справа этот первый тайл */
            MaxBiasCoord.Col = (Bias.X + (TileSize.X - DistanceToAppearanceOfFirstNewTile.X)) / TileSize.X;
        }
        else {
            /* При камере, упревшейся в границу контента, вычислять смещение координат проще - они всегда будут равны друг другу и такому
             * количеству тайлов, какое уложится в расстоянии на которое контент изначально торчал справа за пределами своей области */
            MinBiasCoord.Col = MaxBiasCoord.Col = SizeDifference.X / 2.0 / TileSize.X;

            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The content rests against the right edge of the widget"));
        }
    }
    //Иначе сдвиг видимой области влево
    else {
        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The visible area has moved to the left of the center"));

        //Если камера не уткнулась в край контента, то все изменения происходят в обычном режиме
        if (Bias.X > -(CurrentSizeDifference.X / 2.0 - TileSize.X)) {
            /* Смещение максимальной, а конкретнее правой, координаты при сдвиге камеры спарава налево всегда отвечает за сокрытие лишних правых ячеек.
             * Расстояние до появления первого нового тайла здесь отнимается от ширины смещения, которая здесь всегда нулевая или отрицательная, чтобы
             * учесть, что первый тайл можно сокрыть раньше остальных, а именно в момент набора модуля разницы смещения и расстояния до появления нового
             * тайла равного ширине хотя бы 1 целого тайла. То есть смещению достаточно будет покрыть расстояние, равное модулю разницы между шириной
             * тайла и расстоянием до появления первого тайла, чтобы сокрыть справа этот первый тайл */
            MaxBiasCoord.Col = (Bias.X - DistanceToAppearanceOfFirstNewTile.X) / TileSize.X;
            /* Смещение минимальной, а конкретнее левой, координаты при сдвиге камеры спарава налево всегда отвечает за отображение новых левых ячеек.
             * Разница ширины тайла и расстояния до появления первого нового тайла здесь отнимается от ширины смещения, которая здесь всегда нулевая
             * или отрицательная, чтобы учесть, что первый тайл можно отобразить раньше остальных, а именно в момент набора модуля разницы смещения и
             * остатка от ширины тайла равного ширине хотя бы 1 целого тайла. То есть смещению достаточно будет покрыть расстояние, равное модулю
             * расстояния до появления первого тайла, чтобы отобразить слева этот первый тайл */
            MinBiasCoord.Col = (Bias.X - (TileSize.X - DistanceToAppearanceOfFirstNewTile.X)) / TileSize.X;
        }
        else {
            /* При камере, упревшейся в границу контента, вычислять смещение координат проще - они всегда будут равны друг другу и такому
             * количеству тайлов, какое уложится в модуле расстояния на которое контент изначально торчал слева за пределами своей области */
            MinBiasCoord.Col = MaxBiasCoord.Col = -SizeDifference.X / 2.0 / TileSize.X;

            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The content rests against the left edge of the widget"));
        }
    }

    //Смещение по Y больше 0 означает сдвиг видимой области вверх
    if (Bias.Y > 0) {
        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The visible area has moved to the top of the center"));

        //Если камера не уткнулась в край контента, то все изменения происходят в обычном режиме
        if (Bias.Y < CurrentSizeDifference.Y / 2.0 - TileSize.Y) {
            /* Смещение минимальной, а конкретнее нижней, координаты при сдвиге камеры снизу вверх всегда отвечает за погашение лишних нижних ячеек.
             * Расстояние до появления первого нового тайла здесь прибавляется к высоте смещения, чтобы учесть, что первый тайл можно погасить раньше
             * остальных, а именно в момент набора суммы смещения и расстояния до появления нового тайла равной высоте хотя бы 1 целого тайла. То есть
             * смещению достаточно будет покрыть расстояние, равное разнице между высотой тайла и расстоянием до появления первого тайла, чтобы сокрыть
             * снизу этот первый тайл */
            MinBiasCoord.Row = (Bias.Y + DistanceToAppearanceOfFirstNewTile.Y) / TileSize.Y;
            /* Смещение максимальной, а конкретнее верхней, координаты при сдвиге камеры снизу вверх всегда отвечает за отображение новых верхних ячеек.
             * Разница высоты тайла и расстояния до появления первого нового тайла здесь прибавляется к высоте смещения, чтобы учесть, что первый тайл
             * можно отобразить раньше остальных, а именно в момент набора суммы смещения и остатка от высоты тайла равной высоте хотя бы 1 целого тайла.
             * То есть смещению достаточно будет покрыть расстояние, равное расстоянию до появления первого тайла, чтобы отобразить сверху этот первый тайл */
            MaxBiasCoord.Row = (Bias.Y + (TileSize.Y - DistanceToAppearanceOfFirstNewTile.Y)) / TileSize.Y;
        }
        else {
            /* При камере, упревшейся в границу контента, вычислять смещение координат проще - они всегда будут равны друг другу и такому
             * количеству тайлов, какое уложится в расстоянии на которое контент изначально торчал сверху за пределами своей области */
            MinBiasCoord.Row = MaxBiasCoord.Row = SizeDifference.Y / 2.0 / TileSize.Y;

            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The content rests against the top edge of the widget"));
        }
    }
    //Иначе сдвиг видимой области вниз
    else {
        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The visible area has moved to the bottom of the center"));

        //Если камера не уткнулась в край контента, то все изменения происходят в обычном режиме
        if (Bias.Y > -(CurrentSizeDifference.Y / 2.0 - TileSize.Y)) {
            /* Смещение максимальной, а конкретнее верхней, координаты при сдвиге камеры сверху вниз всегда отвечает за сокрытие лишних верхних ячеек.
             * Расстояние до появления первого нового тайла здесь отнимается от высоты смещения, которая здесь всегда нулевая или отрицательная, чтобы
             * учесть, что первый тайл можно сокрыть раньше остальных, а именно в момент набора модуля разницы смещения и расстояния до появления нового
             * тайла равного высоте хотя бы 1 целого тайла. То есть смещению достаточно будет покрыть расстояние, равное модулю разницы между высотой
             * тайла и расстоянием до появления первого тайла, чтобы сокрыть сверху этот первый тайл */
            MaxBiasCoord.Row = (Bias.Y - DistanceToAppearanceOfFirstNewTile.Y) / TileSize.Y;
            /* Смещение минимальной, а конкретнее нижней, координаты при сдвиге камеры сверху вниз всегда отвечает за отображение новых нижних ячеек.
             * Разница высоты тайла и расстояния до появления первого нового тайла здесь отнимается от высоты смещения, которая здесь всегда нулевая
             * или отрицательная, чтобы учесть, что первый тайл можно отобразить раньше остальных, а именно в момент набора модуля разницы смещения и
             * остатка от высоты тайла равного высоте хотя бы 1 целого тайла. То есть смещению достаточно будет покрыть расстояние, равное модулю
             * расстояния до появления первого тайла, чтобы отобразить снизу этот первый тайл */
            MinBiasCoord.Row = (Bias.Y - (TileSize.Y - DistanceToAppearanceOfFirstNewTile.Y)) / TileSize.Y;
        }
        else {
            /* При камере, упревшейся в границу контента, вычислять смещение координат проще - они всегда будут равны друг другу и такому
             * количеству тайлов, какое уложится в модуле расстояния на которое контент изначально торчал снизу за пределами своей области */
            MinBiasCoord.Row = MaxBiasCoord.Row = -SizeDifference.Y / 2.0 / TileSize.Y;

            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The content rests against the bottom edge of the widget"));
        }
    }

    //Координаты тайлов, отражающие сдвиг контента, записываются в габариты разницы между изначальным положением контента и текущим положением
    FGridDimensions BiasDimentions = FGridDimensions(MinBiasCoord, MaxBiasCoord);

    if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
        UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: With а visible area bias %s, the current bias dimensions are equal to %s"), *Bias.ToString(), *BiasDimentions.ToString());

    CurrentDimensions = OriginalDimensions + BiasDimentions;

    //Габариты, отображающие разницу между изначальным масштабом и текущим
    FGridDimensions ZoomDimentions = FGridDimensions(FGridCoord(0, 0), FGridCoord(0, 0));

    //Множитель масштаба равный 1 считается минимальным и равным значению по умолчанию
    if (ZoomMultiplier != 1)
    {
        //Коорддинаты тайлов, отражающие расширение границ видимого контента из-за его масштабирования, которые будут прибавлены к изначальным координатам для вычисления конечных границ контента
        FGridCoord MinZoomCoord;
        FGridCoord MaxZoomCoord;

        FVector2D CurrentTileSize = TileSize * ZoomMultiplier;

        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: Current tile size is %s"), *CurrentTileSize.ToString());

        /* Разница в размерах между изначальным размером контента и текущим. Поделено на 2 потому, что всегда
           требуется именно разница за одну сторону, например за правую, а не общая разница по координате x. */
        FVector2D SizeDifferenceBetweenOriginalAndCurrentContentSize = (OriginalDimensionsSize - OriginalDimensionsSize * ZoomMultiplier) / 2.0;

        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: Size difference between original and current content size on each side is %s"), *SizeDifferenceBetweenOriginalAndCurrentContentSize.ToString());

        /* Задача оптимизатора забить тайлами разницу в размерах между изначальным размером контента и текущим, то есть забить SizeDifferenceBetweenOriginalAndCurrentContentSize.
         * Но прежду чем начать это делать следует решить пару проблем. Во-первых совпадение размеров контента и его области стопроцентным не будет. Практически всегда контент
         * будет торчать за границами своей области, а так как торчащую часть и так не видно, то и забивать её смысла нет. Чтобы узнать на сколько в данный момент контент торчит
         * из-за пределов своих границ следует умножить изначальный размер DistanceToAppearanceOfFirstNewTile на множитель зума. Это будет не очень точно, но погрешность небольшая
         * и всегда в меньшую сторону, так что не сильно страшно. Полученное расстояние вычетается из изначальной разницы в размерах. Вторая проблема заключается в том, что конент
         * не только масштабируется, но и сдвигается. Если после сдвига активная часть контента расположена не так же по центру, как и была изначально, то меняется и расстояние до
         * отображения нового тайла, при этом с одного края в большую, а с другой - в меньшую сторону. Чтобы учесть этот сдвиг (Bias) надо его прибавить. Но здесь требуется не общий
         * сдвиг, а сдвиг на масштабах одного последнего тайла со всех сторон, то есть такой сдвиг, который остаётся от всех предыдущих появлений новых тайлов от сдвига. Этот остаток,
         * которого уже не хватает для появленя новых тайлов, но который смещает активную часть контента от центра и необходимо прибавить к размеру разницы. Затем полученное расстояние
         * просто делится на текущий размер тайла с округлением вверх, ведь тайл надо отображать даже если он не полностью показался из-за границ виджета. */
        MaxZoomCoord.Col = ceil((SizeDifferenceBetweenOriginalAndCurrentContentSize.X - DistanceToAppearanceOfFirstNewTile.X * ZoomMultiplier +
            (Bias.X - MaxBiasCoord.Col * TileSize.X)) / CurrentTileSize.X);

        //Если по какой-либо причине максимальная коордиата смещения от зума стала меньше 0, то это исправляется. Зум никогда не приблизит контент ближе изначальной нулевой позиции
        if (MaxZoomCoord.Col < 0)
            MaxZoomCoord.Col = 0;

        /* Вычисление отображаемых тайлов от зума у минимальной координаты очень похоже на то, как это происходит у максимальной. Есть лишь пара отличий - вместо прибавления сдвига,
         * он вычитается, а итоговое число координаты делается отрицательным. Это необходимо так как эти координаты будут прибавлены к стартовым, а минимальная координата должна
         * сдвигать активную часть контента влево. И для возникновения этого сдвига показатель MinZoomCoord должен быть отрицательным */
        MinZoomCoord.Col = -ceil((SizeDifferenceBetweenOriginalAndCurrentContentSize.X - DistanceToAppearanceOfFirstNewTile.X * ZoomMultiplier -
            (Bias.X - MinBiasCoord.Col * TileSize.X)) / CurrentTileSize.X);

        //Если по какой-либо причине минимальная коордиата смещения от зума стала больше 0, то это исправляется. Зум никогда не приблизит контент ближе изначальной нулевой позиции
        if (MinZoomCoord.Col > 0)
            MinZoomCoord.Col = 0;
        /* Производится проверка не привысило ли количество отображаемых тайлов реальное количество тайлов. Да, такое вполне может произойти
         * в моменты прокрутки контента близко от края родительского виджета из-за большого количества float'ов и неточного, но быстрого способа
         * рассчёта длины части контента, торчащей из-за пределов своей области. Если же количество тайлов для отображения действительно вышло
         * за пределы реального количества тайлов, то, зная, что такая проблема может возникнуть лишь почти вплотную у края виджета, просто
         * делаются видимыми все тайлы от габаритов смещения до соответствующей стороны контента. Тем самым лишняя часть габаритов зума просто
         * усекается до реальных габаритов контента. */
        if (Bias.X > 0 && MaxZoomCoord.Col + MaxBiasCoord.Col > SizeDifference.X / TileSize.X / 2.0)
            MaxZoomCoord.Col = SizeDifference.X / TileSize.Y / 2.0 - MaxBiasCoord.Col;
        if (Bias.X < 0 && MinZoomCoord.Col + MinBiasCoord.Col < -SizeDifference.X / TileSize.X / 2.0)
            MinZoomCoord.Col = -(SizeDifference.X / TileSize.Y / 2.0 + MinBiasCoord.Col);
        /* расчёты появления новых отображаемых тайлов от зума у координаты Y идентичны расчётам по координате X, где
         * MaxZoomCoord.Y - верх, который считается как право, а MinZoomCoord.Y - низ, который считается как лево */
        MaxZoomCoord.Row = ceil((SizeDifferenceBetweenOriginalAndCurrentContentSize.Y - DistanceToAppearanceOfFirstNewTile.Y * ZoomMultiplier +
            (Bias.Y - MaxBiasCoord.Row * TileSize.Y)) / CurrentTileSize.Y);

        if (MaxZoomCoord.Row < 0)
            MaxZoomCoord.Row = 0;

        MinZoomCoord.Row = -ceil((SizeDifferenceBetweenOriginalAndCurrentContentSize.Y - DistanceToAppearanceOfFirstNewTile.Y * ZoomMultiplier -
            (Bias.Y - MinBiasCoord.Row * TileSize.Y)) / CurrentTileSize.Y);

        if (MinZoomCoord.Row > 0)
            MinZoomCoord.Row = 0;

        if (Bias.Y > 0 && MaxZoomCoord.Row + MaxBiasCoord.Row > SizeDifference.Y / TileSize.Y / 2.0)
            MaxZoomCoord.Row = SizeDifference.Y / TileSize.Y / 2.0 - MaxBiasCoord.Row;
        if (Bias.Y < 0 && MinZoomCoord.Row + MinBiasCoord.Row < -SizeDifference.Y / TileSize.Y / 2.0)
            MinZoomCoord.Row = -(SizeDifference.Y / TileSize.Y / 2.0 + MinBiasCoord.Row);

        //Координаты тайлов, отражающие масштабирование контента, записываются в габариты разницы между изначальным размером контента и его текущим размером
        ZoomDimentions = FGridDimensions(MinZoomCoord, MaxZoomCoord);

        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: With a zoom multiplier of %f, the current zoom dimensions are equal to %s"), ZoomMultiplier, *ZoomDimentions.ToString());
    }
    else if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
        UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: ZoomMultiplier is equal to 1 - no scaling"));

    CurrentDimensions = CurrentDimensions + ZoomDimentions;

    /* Бывают моменты, когда размер контента меньше минимального. В этом случае нужные размеры виджету контента помогает
     * держать специальная подложка, но размер меньше минимального так же говорит и о том, что все рассчёты по габаритам
     * были не нужны, а настоящая рабочая область контента просто меньше размера своего виджета, так что по соответствующей
     * координате все тайлы должны быть всегда показаны */
    bool CurrentXLessThanMin = MaximumContentSize.X < MinContentSize.X;
    bool CurrentYLessThanMin = MaximumContentSize.Y < MinContentSize.Y;

    if (TilesCoordWrapper->HasAnyEllements()) {
        //Если одна из осей контента меньше минимального размера, то отображаются все тайлы по соответствующей оси
        if (CurrentXLessThanMin) {
            CurrentDimensions.Min.Col = 0;
            CurrentDimensions.Max.Col = NumberOfTileColsInTable - 1;

            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The current width is less than the minimum"));
        }

        if (CurrentYLessThanMin) {
            CurrentDimensions.Min.Row = 0;
            CurrentDimensions.Max.Row = NumberOfTileRowsInTable - 1;

            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The current height is less than the minimum"));
        }

        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: Current final dimensions of the visible content area is %s"), *CurrentDimensions.ToString());

        //Изменения производятся если габариты контента изменились
        if (OldDimensions != CurrentDimensions) {
            //UE_LOG(TileTablesOptimizationTools, Log, TEXT("old: %s, new %s"), *OldDimensions.ToString(), *CurrentDimensions.ToString());

            if (OldDimensions.DoTheDimensionsIntersect(CurrentDimensions)) {
                //Чтобы таргетировано убирать или добавлять строки и стобцы, проверяется как именно изменились габариты контента
                bool NewTopBoundMore = (CurrentDimensions.Max.Row > OldDimensions.Max.Row);
                bool OldTopBoundLess = CurrentDimensions.Max.Row < OldDimensions.Max.Row;
                bool NewBottomBoundMore = CurrentDimensions.Min.Row < OldDimensions.Min.Row;
                bool OldBottomBoundLess = CurrentDimensions.Min.Row > OldDimensions.Min.Row;
                bool NewLeftBoundMore = CurrentDimensions.Min.Col < OldDimensions.Min.Col;
                bool OldLeftBoundLess = CurrentDimensions.Min.Col > OldDimensions.Min.Col;
                bool NewRightBoundMore = CurrentDimensions.Max.Col > OldDimensions.Max.Col;
                bool OldRightBoundLess = CurrentDimensions.Max.Col < OldDimensions.Max.Col;

                //Если размер контента по Y меньше минимального, то это означает, что все тайлы по вертикали и так всегда отображены, и нет смысла гонять эти проверки и цыклы
                if (!CurrentYLessThanMin) {
                    //Если было зарегистрировано, что новая текущая область выше верхней границы предыдущей области, то все необходимые новые строки становятся видимыми
                    if (NewTopBoundMore) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is higher than the top of the previous area"));
                        if (!NewLeftBoundMore && !NewRightBoundMore) {
                            /* Так как известно, что новая область выше предыдущей, цикл стартует со строки на 1 выше, чем предыдущая верхняя
                             * граница, чтобы затронуть только неотображённые тайлы, и проходит до новой верхней границы включительно */
                            AsynchronousAreaFilling(FGridDimensions(FGridCoord(OldDimensions.Max.Row + 1, CurrentDimensions.Min.Col),
                                FGridCoord(CurrentDimensions.Max.Row, CurrentDimensions.Max.Col)), NumberOfMapTilesRows, "1");
                        }
                        else {
                            if (NewLeftBoundMore) {
                                AsynchronousAreaFilling(FGridDimensions(FGridCoord(OldDimensions.Max.Row + 1, OldDimensions.Min.Col),
                                    FGridCoord(CurrentDimensions.Max.Row, CurrentDimensions.Max.Col)), NumberOfMapTilesRows, "1.1");
                            }

                            if (NewRightBoundMore) {
                                AsynchronousAreaFilling(FGridDimensions(FGridCoord(OldDimensions.Max.Row + 1, CurrentDimensions.Min.Col),
                                    FGridCoord(CurrentDimensions.Max.Row, OldDimensions.Max.Col)), NumberOfMapTilesRows, "1.2");
                            }
                        }
                    }

                    //Если было зарегистрировано, что новая текущая область ниже верхней границы предыдущей области, то все необходимые старые строки становятся сколлапсированными
                    if (OldTopBoundLess) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is below the top of the previous area"));

                        if (!OldLeftBoundLess && !OldRightBoundLess) {
                            /* Так как известно, что новая область ниже предыдущей, цикл стартует со строки на 1 выше, чем текущая верхняя
                             * граница, чтобы затронуть только отображённые тайлы, и проходит до старой верхней границы включительно */
                            AsynchronousAreaRemoving(FGridDimensions(FGridCoord(CurrentDimensions.Max.Row + 1, OldDimensions.Min.Col),
                                FGridCoord(OldDimensions.Max.Row, OldDimensions.Max.Col)), NumberOfMapTilesRows, "1");
                        }
                        else {
                            if (OldLeftBoundLess) {
                                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(CurrentDimensions.Max.Row + 1, CurrentDimensions.Min.Col),
                                    FGridCoord(OldDimensions.Max.Row, OldDimensions.Max.Col)), NumberOfMapTilesRows, "1.1");
                            }

                            if (OldRightBoundLess) {
                                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(CurrentDimensions.Max.Row + 1, OldDimensions.Min.Col),
                                    FGridCoord(OldDimensions.Max.Row, CurrentDimensions.Max.Col)), NumberOfMapTilesRows, "1.2");
                            }
                        }
                    }

                    //Если было зарегистрировано, что новая текущая область ниже нижней границы предыдущей области, то все необходимые новые строки становятся видимыми
                    if (NewBottomBoundMore) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is below the lower boundary of the previous area"));

                        if (!NewLeftBoundMore && !NewRightBoundMore) {
                            /* Так как известно, что новая область ниже предыдущей, цикл стартует со строки равной текущей нижней границе, и проходит
                             * до строки, расположенной на 1 ниже, чем старая нижняя граница, чтобы затронуть только неотображённые тайлы */
                            AsynchronousAreaFilling(FGridDimensions(FGridCoord(CurrentDimensions.Min.Row, CurrentDimensions.Min.Col),
                                FGridCoord(OldDimensions.Min.Row - 1, CurrentDimensions.Max.Col)), NumberOfMapTilesRows, "2");
                        }
                        else {
                            if (NewLeftBoundMore) {
                                AsynchronousAreaFilling(FGridDimensions(FGridCoord(CurrentDimensions.Min.Row, OldDimensions.Min.Col),
                                    FGridCoord(OldDimensions.Min.Row - 1, CurrentDimensions.Max.Col)), NumberOfMapTilesRows, "2.1");
                            }

                            if (NewRightBoundMore) {
                                AsynchronousAreaFilling(FGridDimensions(FGridCoord(CurrentDimensions.Min.Row, CurrentDimensions.Min.Col),
                                    FGridCoord(OldDimensions.Min.Row - 1, OldDimensions.Max.Col)), NumberOfMapTilesRows, "2.2");
                            }
                        }
                    }

                    //Если было зарегистрировано, что новая текущая область выше нижней границы предыдущей области, то все необходимые старые строки становятся сколлапсированными
                    if (OldBottomBoundLess) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is higher than the lower border of the previous area"));

                        if (!OldLeftBoundLess && !OldRightBoundLess) {
                            /* Так как известно, что новая область выше предыдущей, цикл стартует со строки равной старой нижней границе, и
                             * проходит до строки, расположенной на 1 ниже, чем текущая нижняя граница, чтобы затронуть только отображённые тайлы */
                            AsynchronousAreaRemoving(FGridDimensions(FGridCoord(OldDimensions.Min.Row, OldDimensions.Min.Col),
                                FGridCoord(CurrentDimensions.Min.Row - 1, OldDimensions.Max.Col)), NumberOfMapTilesRows, "2");
                        }
                        else {
                            if (OldLeftBoundLess) {
                                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(OldDimensions.Min.Row, CurrentDimensions.Min.Col),
                                    FGridCoord(CurrentDimensions.Min.Row - 1, OldDimensions.Max.Col)), NumberOfMapTilesRows, "2.1");
                            }
                            if (OldRightBoundLess) {
                                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(OldDimensions.Min.Row, OldDimensions.Min.Col),
                                    FGridCoord(CurrentDimensions.Min.Row - 1, CurrentDimensions.Max.Col)), NumberOfMapTilesRows, "2.2");
                            }
                        }
                    }
                }

                //Если размер контента по X меньше минимального, то это означает, что все тайлы по горизонтали и так всегда отображены, и нет смысла гонять эти проверки и циклы
                if (!CurrentXLessThanMin) {
                    //Если было зарегистрировано, что новая текущая область левее левой границы предыдущей области, то все необходимые новые столбцы становятся видимыми
                    if (NewLeftBoundMore) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The current area is to the left of the left border of the previous area"));

                        AsynchronousAreaFilling(FGridDimensions(FGridCoord(CurrentDimensions.Min.Row, CurrentDimensions.Min.Col),
                            FGridCoord(CurrentDimensions.Max.Row, OldDimensions.Min.Col - 1)), NumberOfMapTilesRows, "3");
                    }

                    //Если было зарегистрировано, что новая текущая область правее левой границы предыдущей области, то все необходимые старые столбцы становятся сколлапсированными
                    if (OldLeftBoundLess) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is to the right of the left border of the previous area"));

                        /* Так как известно, что новая область правее предыдущей, цикл стартует со столбца равного старой левой границе, и
                         * проходит до столбца, расположенного на 1 левее, чем текущая левая граница, чтобы затронуть только отображённые тайлы */
                        AsynchronousAreaRemoving(FGridDimensions(FGridCoord(OldDimensions.Min.Row, OldDimensions.Min.Col),
                            FGridCoord(OldDimensions.Max.Row, CurrentDimensions.Min.Col - 1)), NumberOfMapTilesRows, "3");
                    }

                    //Если было зарегистрировано, что новая текущая область правее правой границы предыдущей области, то все необходимые новые столбцы становятся видимыми
                    if (NewRightBoundMore) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is to the right of the right border of the previous area"));

                        /* Так как известно, что новая область правее предыдущей, цикл стартует со столбца на 1 правее, чем предыдущая правая
                         * граница, чтобы затронуть только неотображённые тайлы, и проходит до новой правой границы включительно */
                        AsynchronousAreaFilling(FGridDimensions(FGridCoord(CurrentDimensions.Min.Row, OldDimensions.Max.Col + 1),
                            FGridCoord(CurrentDimensions.Max.Row, CurrentDimensions.Max.Col)), NumberOfMapTilesRows, "4");
                    }

                    //Если было зарегистрировано, что новая текущая область левее правой границы предыдущей области, то все необходимые старые столбцы становятся сколлапсированными
                    if (OldRightBoundLess) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is to the left of the right border of the previous area"));

                        /* Так как известно, что новая область левее предыдущей, цикл стартует со столбца на 1 правее, чем текущая правая
                         * граница, чтобы затронуть только отображённые тайлы, и проходит до старой правой границы включительно */
                        AsynchronousAreaRemoving(FGridDimensions(FGridCoord(OldDimensions.Min.Row, CurrentDimensions.Max.Col + 1),
                            FGridCoord(OldDimensions.Max.Row, OldDimensions.Max.Col)), NumberOfMapTilesRows, "3");
                    }
                }
            }
            else {
                AsynchronousAreaFilling(CurrentDimensions, NumberOfMapTilesRows, "all");
                AsynchronousAreaRemoving(OldDimensions, NumberOfMapTilesRows, "all");
            }

            TilesCoordWrapper->setMinCoord(CurrentDimensions.Min);
            TilesCoordWrapper->setMaxCoord(CurrentDimensions.Max);
        }
        else {
            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The current content dimensions do not differ from the previous ones"));
        }
    }
    else
        UE_LOG(LogTemp, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: TilesCoordWrapper does not include any elements"));

    OldDimensions = CurrentDimensions;
}