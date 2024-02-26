// Denis Igorevich Frolov did all this. Once there. All things reserved.

#include "RoguelikeThing/Function Libraries/TileTablesOptimizationTools.h"

DEFINE_LOG_CATEGORY(TileTablesOptimizationTools);

/* Первичная инициализация таблицы тайлов таким образом, чтобы в ней видимыми
 * были тольо те тайлы, которые влезли в область родительского виджета */
FVector2D UTileTablesOptimizationTools::InitTableTiles(UCoordWrapperOfTable* TilesCoordWrapper,
    FVector2D TileSize, FVector2D WidgetAreaSize, FMapDimensions MapDimensions, FVector2D MinContentSize)
{
    if (!TilesCoordWrapper) {
        UE_LOG(TileTablesOptimizationTools, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the InitTableTiles function: TilesCoordWrapper is not valid"));
        InitializationValidity = FInitializationValidity::ERROR;
        return FVector2D();
    }
    if (MapDimensions.isValid) {
        if (TilesCoordWrapper->HasAnyEllements()) {
            minContentSize = MinContentSize;
            TileLen = MapDimensions.MapTileLength;
            //Количество тайлов в стороне матрицы фрагмента вычисляется делением длины стороны фрагмента на длину тайла
            int TilesInFragmentLen = MapDimensions.TableLength / TileLen;

            /* Для вычисления количества строк тайлов в тайловой таблице сначала находится количество строк фрагментов.
             * Так как минимальная координата фрагментов не обязательно должна быть 0:0, общее количество строк находится
             * путём вычитания из наибольшей координаты наименьшей. Но так как вычисления производились с индексами, у
             * которых отсчёт ведётся с 0, то полученное значение следует привести к порядковому номеру прибавив единицу.
             * Затем полученное количество строк фрагментов просто умножается на количество тайлов в стороне матрицы фрагментов */
            TableTileRows = (MapDimensions.MaxRow - MapDimensions.MinRow + 1) * TilesInFragmentLen;
            //Количество столбцов ищется также, как и количество строк
            TableTileCols = (MapDimensions.MaxCol - MapDimensions.MinCol + 1) * TilesInFragmentLen;

            //Запоминается реальное количество строк и столбцов, так как отображаемое количество может не совпадать
            RealTableTileRows = TableTileRows;
            RealTableTileCols = TableTileCols;

            //Таблица не отображает более 3 фрагментов в строках или столбцах за раз. Это сделано для оптимизации
            if (TableTileRows > 3 * TilesInFragmentLen)
                TableTileRows = 3 * TilesInFragmentLen;
            if (TableTileCols > 3 * TilesInFragmentLen)
                TableTileCols = 3 * TilesInFragmentLen;

            /* Запоминаются те размеры, которыми изначально обладали виджеты тайла и области
             * контента. Это понадобится позже для вычисления масштабирования и сдвига */
            OriginalTileSize = TileSize;
            widgetAreaSize = WidgetAreaSize;

            //Размеры полученной таблицы легко вычислить умножив размеры тайла на количество столбцов и строк
            OriginalTableSize = OriginalTileSize * FVector2D(TableTileCols, TableTileRows);
            //Вычисляется разница в размерах между тайловой таблицей и окном, в который она полмещена
            SizeDifference = OriginalTableSize - widgetAreaSize;

            /* Если размер тайловой таблицы меньше, чем минимальный размер, предусмтренный для виджета области
             * контента, то разница размеров игнорируется, здесь необходимо учитывать только положительную разницу */
            if (OriginalTableSize.X < minContentSize.X)
                SizeDifference.X = 0;
            if (OriginalTableSize.Y < minContentSize.Y)
                SizeDifference.Y = 0;

            //На основе разницы размеров вычисляется сколько строк и с столбцов должны быть скрыты сверху и снизу, справа и слева
            int NumberOfCollapsedTilesTopAndBottom = (SizeDifference.Y / 2.0) / OriginalTileSize.Y;
            int NumberOfCollapsedTilesRinhtAndLeft = (SizeDifference.X / 2.0) / OriginalTileSize.X;

            /* Если по каким-то причинам число скрытых столбцов или строк отрицательное, то
             * этот параметр игнорируется. Отрицательные значения здесь ломают логику функции */
            if (NumberOfCollapsedTilesTopAndBottom < 0)
                NumberOfCollapsedTilesTopAndBottom = 0;
            if (NumberOfCollapsedTilesRinhtAndLeft < 0)
                NumberOfCollapsedTilesRinhtAndLeft = 0;

            /***************************************************************/UE_LOG(LogTemp, Log, TEXT("%d, %d"), NumberOfCollapsedTilesTopAndBottom, NumberOfCollapsedTilesRinhtAndLeft);//!!!!

            /* Запоминаются текущие габариты отображаемой области. Минимальная точка будет сразу же после левых
             * нижних скрытых строк и столбцов, и так как искомое число - это индекс, порядковый номер нужно
             * привести к нему вычитанием единицы. Но данное приведение здесь опускается, так как отсчёт идёт
             * после скрытых тайлов, а не на них, то есть к данной точке потребуется ещё и прибавить единицу.
             * В максимальной же точке вычитание единицы необходимо, так как эта координата находится в правом
             * верхнем уголу матрицы, то есть "сразу после скрытых строк и столбцов" для этой точки будет на
             * один тайл снизу слева относительно грани скрытых тайлов, а не справа сверху, как у минимальной точки */
            CurrentDimensions = FDimensionsDisplayedArea(
                FTileCoord(NumberOfCollapsedTilesRinhtAndLeft, NumberOfCollapsedTilesTopAndBottom),
                FTileCoord(TableTileCols - NumberOfCollapsedTilesRinhtAndLeft - 1, TableTileRows - NumberOfCollapsedTilesTopAndBottom - 1));

            /***************************************************************/UE_LOG(LogTemp, Log, TEXT("CurrentDimensions: %s"), *CurrentDimensions.ToString());//!!!!

            //Все тыйлы, входящие в область текущей видимости делаются видимыми
            for (int row = CurrentDimensions.Min.Y; row <= CurrentDimensions.Max.Y; row++) {
                for (int col = CurrentDimensions.Min.X; col <= CurrentDimensions.Max.X; col++) {
                    UWidget* GridPanelElement = TilesCoordWrapper->FindWidget(row, col);
                    if (GridPanelElement)
                        GridPanelElement->SetVisibility(ESlateVisibility::Visible);
                    else {
                        UE_LOG(TileTablesOptimizationTools, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the InitTableTiles function: Tile Row:%d Col:%d is not valid"), row, col);
                        InitializationValidity = FInitializationValidity::ERROR;
                        return FVector2D();
                    }
                }
            }

            /* Схлопнутые при комощи Collapsed тайлы тратят меньше ресурсов компьютера, чем скрытые с помощью Hidden,
             * так что по умолчанию все тайлы, кроме видимых в данный момент схлопнуты. Но из-за этого габариты таблицы
             * сжимаются до габаритов видимой области, а если жёстко задать размер таблице, то тайлы видимой области
             * просто равномерно распределяются по всей её площади. Решить это можно просто - следует сделань скрытыми
             * (Hidden), а не схлопнутыми (Collapsed) тайлы с наибольшей и наименьшей координатами. Эти тайлы как бы
             * "натягивают" всю таблицу, находясь в её двух противоположных углах */

            GridCoord MinCoord = TilesCoordWrapper->getMinCoord();
            if (MinCoord.getIsInit()) {
                UWidget* HiddenWidget = TilesCoordWrapper->FindWidget(MinCoord.Row, MinCoord.Col);
                /* Если крайний тайл не схлопнут, это значит, что размер всей таблицы не превышает размеров видимой 
                 * облати, и следовательно этот тайл видим. В таком случае наобности в "натяжении" таблицы нет */
                if (HiddenWidget && HiddenWidget->GetVisibility() == ESlateVisibility::Collapsed)
                    HiddenWidget->SetVisibility(ESlateVisibility::Hidden);
            }
            else{
                UE_LOG(TileTablesOptimizationTools, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the InitTableTiles function: Min coord tile Row:%d Col:%d is not initialized"), MinCoord.Row, MinCoord.Col);
                InitializationValidity = FInitializationValidity::ERROR;
                return FVector2D();
            }

            GridCoord MaxCoord = TilesCoordWrapper->getMaxCoord();
            if (MaxCoord.getIsInit()) {
                UWidget* HiddenWidget = TilesCoordWrapper->FindWidget(MaxCoord.Row, MaxCoord.Col);
                /* Если крайний тайл не схлопнут, это значит, что размер всей таблицы не превышает размеров видимой
                 * облати, и следовательно этот тайл видим. В таком случае наобности в "натяжении" таблицы нет */
                if (HiddenWidget && HiddenWidget->GetVisibility() == ESlateVisibility::Collapsed)
                    HiddenWidget->SetVisibility(ESlateVisibility::Hidden);
            }
            else {
                UE_LOG(TileTablesOptimizationTools, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the InitTableTiles function: Max coord tile Row:%d Col:%d is not initialized"), MaxCoord.Row, MaxCoord.Col);
                InitializationValidity = FInitializationValidity::ERROR;
                return FVector2D();
            }

            //На данном этапе текущие габариты видимой области и есть изначальные габариты
            OriginalDimensions = CurrentDimensions;
            //Габариты, отображающие размер видимой области на предыдущем срабатывании функции изначально инициализируются стартовыми габаритами
            OldDimensions = OriginalDimensions;

            //Реальные размеры, записанные в Vector2D, для габаритов изначально видимой области. Прибавленые единички - это приведение индекса к порядковому номеру
            OriginalDimensionsSize = FVector2D((OriginalDimensions.Max.X - OriginalDimensions.Min.X + 1) * OriginalTileSize.X,
                (OriginalDimensions.Max.Y - OriginalDimensions.Min.Y + 1) * OriginalTileSize.Y);

            /* Расстояние, при преодолении которого появляется первый новый тайл вычисляется путём нахождения половины разницы
             * между размерами изначально видимой области и области отображаемого контента. Тем самым находинся вектор, равный
             * такому размеру, который как бы торчит за пределами виджета, внутрь которого помещена тайловая таблица */
            DistanceToAppearanceOfFirstNewTile = (OriginalDimensionsSize - widgetAreaSize) / 2.0;
            /* Если размер тайловой таблицы меньше, чем виджет, в который она помещена, то показатель расстояния до появления
             * нового первого тайла может стать отрицательным. В таком случае, никаких новых тайлов за пределами виджета нет, и
             * появляться нечему. Пременная приравнивается к 0, что будет индикатором того, что ничего грузить не надо */
            if (DistanceToAppearanceOfFirstNewTile.X < 0)
                DistanceToAppearanceOfFirstNewTile.X = 0;
            if (DistanceToAppearanceOfFirstNewTile.Y < 0)
                DistanceToAppearanceOfFirstNewTile.Y = 0;

            InitializationValidity = FInitializationValidity::VALID;
            return OriginalTableSize;
        }
        else {
            UE_LOG(TileTablesOptimizationTools, Warning, TEXT("Warning in TileTablesOptimizationTools class in InitTableTiles function - TilesCoordWrapper does not include any elements, the function will not do anything"));
            InitializationValidity = FInitializationValidity::ERROR;
            return FVector2D();
        }
    }
    else
    {
        UE_LOG(TileTablesOptimizationTools, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the InitTableTiles function: MapDimensions is not valid"));
        InitializationValidity = FInitializationValidity::ERROR;
        return FVector2D();
    }
}

//Функция, изменяющая видимость тайлов от сдвига или масштабирования таблицы
void UTileTablesOptimizationTools::ChangingVisibilityOfTableTiles(UCoordWrapperOfTable* TilesCoordWrapper, FVector2D Bias, float ZoomMultiplier)
{
    //Проверяется валидность проведённой инициализации таблицы
    switch (InitializationValidity)
    {
    case FInitializationValidity::WAS_NOT_PERFORMED:
        UE_LOG(TileTablesOptimizationTools, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The tile table has not been initialized, optimization is impossible! Always call the InitTableTiles function once before starting to work with the functionality of the TileTablesOptimizationTools library"));
        return;
        break;
    case FInitializationValidity::VALID:
        break;
    case FInitializationValidity::ERROR:
        UE_LOG(TileTablesOptimizationTools, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: Tile table initialization completed with errors (see logs above). Optimization is not possible until errors that occur during initialization are corrected"));
        return;
        break;
    default:
        break;
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

    //Коорддинаты тайлов, отражающие сдвиг контента, которые будут прибавлены к изначальным координатам для вычисления конечной позиции контента
    FTileCoord MinBiasCoord;
    FTileCoord MaxBiasCoord;

    //Смещение по X больше 0 означает сдвиг видимой области вправо
    if (Bias.X > 0) {
        //Если камера не уткнулась в край контента, то все изменения происходят в обычном режиме
        if (Bias.X < SizeDifference.X / 2.0) {
            /* Смещение минимальной, а конкретнее левой, координаты при сдвиге камеры слева направо всегда отвечает за погашение лишних левых ячеек.
             * Расстояние до появления первого нового тайла здесь прибавляется к ширине смещения, чтобы учесть, что первый тайл можно погасить раньше
             * остальных, а именно в момент набора суммы смещения и расстояния до появления нового тайла равной ширине хотя бы 1 целого тайла. То есть
             * смещению достаточно будет покрыть расстояние, равное разнице между шириной тайла и расстоянием до появления первого тайла, чтобы сокрыть
             * слева этот первый тайл */
            MinBiasCoord.X = (Bias.X + DistanceToAppearanceOfFirstNewTile.X) / OriginalTileSize.X;
            /* Смещение максимальной, а конкретнее правой, координаты при сдвиге камеры слева направо всегда отвечает за отображение новых правых ячеек.
             * Разница ширины тайла и расстояния до появления первого нового тайла здесь прибавляется к ширине смещения, чтобы учесть, что первый тайл
             * можно отобразить раньше остальных, а именно в момент набора суммы смещения и остатка от ширины тайла равной ширине хотя бы 1 целого тайла.
             * То есть смещению достаточно будет покрыть расстояние, равное расстоянию до появления первого тайла, чтобы отобразить справа этот первый тайл */
            MaxBiasCoord.X = (Bias.X + (OriginalTileSize.X - DistanceToAppearanceOfFirstNewTile.X)) / OriginalTileSize.X;
        }
        else
            /* При камере, упревшейся в границу контента, вычислять смещение координат проще - они всегда будут равны друг другу и такому
             * количеству тайлов, какое уложится в расстоянии на которое контент изначально торчал справа за пределами своей области */
            MinBiasCoord.X = MaxBiasCoord.X = SizeDifference.X / 2.0 / OriginalTileSize.X;
    }
    //Иначе сдвиг видимой области влево
    else {
        //Если камера не уткнулась в край контента, то все изменения происходят в обычном режиме
        if (Bias.X > -(SizeDifference.X / 2.0)) {
            /* Смещение максимальной, а конкретнее правой, координаты при сдвиге камеры спарава налево всегда отвечает за сокрытие лишних правых ячеек.
             * Расстояние до появления первого нового тайла здесь отнимается от ширины смещения, которая здесь всегда нулевая или отрицательная, чтобы
             * учесть, что первый тайл можно сокрыть раньше остальных, а именно в момент набора модуля разницы смещения и расстояния до появления нового
             * тайла равного ширине хотя бы 1 целого тайла. То есть смещению достаточно будет покрыть расстояние, равное модулю разницы между шириной
             * тайла и расстоянием до появления первого тайла, чтобы сокрыть справа этот первый тайл */
            MaxBiasCoord.X = (Bias.X - DistanceToAppearanceOfFirstNewTile.X) / OriginalTileSize.X;
            /* Смещение минимальной, а конкретнее левой, координаты при сдвиге камеры спарава налево всегда отвечает за отображение новых левых ячеек.
             * Разница ширины тайла и расстояния до появления первого нового тайла здесь отнимается от ширины смещения, которая здесь всегда нулевая
             * или отрицательная, чтобы учесть, что первый тайл можно отобразить раньше остальных, а именно в момент набора модуля разницы смещения и
             * остатка от ширины тайла равного ширине хотя бы 1 целого тайла. То есть смещению достаточно будет покрыть расстояние, равное модулю
             * расстояния до появления первого тайла, чтобы отобразить слева этот первый тайл */
            MinBiasCoord.X = (Bias.X - (OriginalTileSize.X - DistanceToAppearanceOfFirstNewTile.X)) / OriginalTileSize.X;
        }
        else
            /* При камере, упревшейся в границу контента, вычислять смещение координат проще - они всегда будут равны друг другу и такому
             * количеству тайлов, какое уложится в модуле расстояния на которое контент изначально торчал слева за пределами своей области */
            MinBiasCoord.X = MaxBiasCoord.X = -SizeDifference.X / 2.0 / OriginalTileSize.X;
    }

    //Смещение по Y больше 0 означает сдвиг видимой области вверх
    if (Bias.Y > 0) {
        //Если камера не уткнулась в край контента, то все изменения происходят в обычном режиме
        if (Bias.Y < SizeDifference.Y / 2.0) {
            /* Смещение минимальной, а конкретнее нижней, координаты при сдвиге камеры снизу вверх всегда отвечает за погашение лишних нижних ячеек.
             * Расстояние до появления первого нового тайла здесь прибавляется к высоте смещения, чтобы учесть, что первый тайл можно погасить раньше
             * остальных, а именно в момент набора суммы смещения и расстояния до появления нового тайла равной высоте хотя бы 1 целого тайла. То есть
             * смещению достаточно будет покрыть расстояние, равное разнице между высотой тайла и расстоянием до появления первого тайла, чтобы сокрыть
             * снизу этот первый тайл */
            MinBiasCoord.Y = (Bias.Y + DistanceToAppearanceOfFirstNewTile.Y) / OriginalTileSize.Y;
            /* Смещение максимальной, а конкретнее верхней, координаты при сдвиге камеры снизу вверх всегда отвечает за отображение новых верхних ячеек.
             * Разница высоты тайла и расстояния до появления первого нового тайла здесь прибавляется к высоте смещения, чтобы учесть, что первый тайл
             * можно отобразить раньше остальных, а именно в момент набора суммы смещения и остатка от высоты тайла равной высоте хотя бы 1 целого тайла.
             * То есть смещению достаточно будет покрыть расстояние, равное расстоянию до появления первого тайла, чтобы отобразить сверху этот первый тайл */
            MaxBiasCoord.Y = (Bias.Y + (OriginalTileSize.Y - DistanceToAppearanceOfFirstNewTile.Y)) / OriginalTileSize.Y;
        }
        else
            /* При камере, упревшейся в границу контента, вычислять смещение координат проще - они всегда будут равны друг другу и такому
             * количеству тайлов, какое уложится в расстоянии на которое контент изначально торчал сверху за пределами своей области */
            MinBiasCoord.Y = MaxBiasCoord.Y = SizeDifference.Y / 2.0 / OriginalTileSize.Y;
    }
    //Иначе сдвиг видимой области вниз
    else {
        //Если камера не уткнулась в край контента, то все изменения происходят в обычном режиме
        if (Bias.Y > -(SizeDifference.Y / 2.0)) {
            /* Смещение максимальной, а конкретнее верхней, координаты при сдвиге камеры сверху вниз всегда отвечает за сокрытие лишних верхних ячеек.
             * Расстояние до появления первого нового тайла здесь отнимается от высоты смещения, которая здесь всегда нулевая или отрицательная, чтобы
             * учесть, что первый тайл можно сокрыть раньше остальных, а именно в момент набора модуля разницы смещения и расстояния до появления нового
             * тайла равного высоте хотя бы 1 целого тайла. То есть смещению достаточно будет покрыть расстояние, равное модулю разницы между высотой
             * тайла и расстоянием до появления первого тайла, чтобы сокрыть сверху этот первый тайл */
            MaxBiasCoord.Y = (Bias.Y - DistanceToAppearanceOfFirstNewTile.Y) / OriginalTileSize.Y;
            /* Смещение минимальной, а конкретнее нижней, координаты при сдвиге камеры сверху вниз всегда отвечает за отображение новых нижних ячеек.
             * Разница высоты тайла и расстояния до появления первого нового тайла здесь отнимается от высоты смещения, которая здесь всегда нулевая
             * или отрицательная, чтобы учесть, что первый тайл можно отобразить раньше остальных, а именно в момент набора модуля разницы смещения и
             * остатка от высоты тайла равного высоте хотя бы 1 целого тайла. То есть смещению достаточно будет покрыть расстояние, равное модулю
             * расстояния до появления первого тайла, чтобы отобразить снизу этот первый тайл */
            MinBiasCoord.Y = (Bias.Y - (OriginalTileSize.Y - DistanceToAppearanceOfFirstNewTile.Y)) / OriginalTileSize.Y;
        }
        else
            /* При камере, упревшейся в границу контента, вычислять смещение координат проще - они всегда будут равны друг другу и такому
             * количеству тайлов, какое уложится в модуле расстояния на которое контент изначально торчал снизу за пределами своей области */
            MinBiasCoord.Y = MaxBiasCoord.Y = -SizeDifference.Y / 2.0 / OriginalTileSize.Y;
    }

    //Координаты тайлов, отражающие сдвиг контента, записываются в габариты разницы между изначальным положением контента и текущим положением
    FDimensionsDisplayedArea BiansDimentions = FDimensionsDisplayedArea(MinBiasCoord, MaxBiasCoord);

    CurrentDimensions = OriginalDimensions + BiansDimentions;

    //Габариты, отображающие разницу между изначальным масштабом и текущим
    FDimensionsDisplayedArea ZoomDimentions = FDimensionsDisplayedArea();

    //Множитель масштаба равный 1 считается минимальным и равным значению по умолчанию
    if (ZoomMultiplier != 1)
    {
        //Коорддинаты тайлов, отражающие расширение границ видимого контента из-за его масштабирования, которые будут прибавлены к изначальным координатам для вычисления конечных границ контента
        FTileCoord MinZoomCoord;
        FTileCoord MaxZoomCoord;

        FVector2D CurrentTileSize = OriginalTileSize * ZoomMultiplier;
        /* Разница в размерах между изначальным размером контента и текущим. Поделено на 2 потому, что всегда
           требуется именно разница за одну сторону, например за правую, а не общая разница по координате x. */
        FVector2D SizeDifferenceBetweenOriginalAndCurrentContentSize = (OriginalDimensionsSize - OriginalDimensionsSize * ZoomMultiplier) / 2.0;

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
        MaxZoomCoord.X = ceil((SizeDifferenceBetweenOriginalAndCurrentContentSize.X - DistanceToAppearanceOfFirstNewTile.X * ZoomMultiplier +
            (Bias.X - MaxBiasCoord.X * OriginalTileSize.X)) / CurrentTileSize.X);

        //Если по какой-либо причине максимальная коордиата смещения от зума стала меньше 0, то это исправляется. Зум никогда не приблизит контент ближе изначальной нулевой позиции
        if (MaxZoomCoord.X < 0)
            MaxZoomCoord.X = 0;

        /* Вычисление отображаемых тайлов от зума у минимальной координаты очень похоже на то, как это происходит у максимальной. Есть лишь пара отличий - вместо прибавления сдвига,
         * он вычитается, а итоговое число координаты делается отрицательным. Это необходимо так как эти координаты будут прибавлены к стартовым, а минимальная координата должна
         * сдвигать активную часть контента влево. И для возникновения этого сдвига показатель MinZoomCoord должен быть отрицательным */
        MinZoomCoord.X = -ceil((SizeDifferenceBetweenOriginalAndCurrentContentSize.X - DistanceToAppearanceOfFirstNewTile.X * ZoomMultiplier -
            (Bias.X - MinBiasCoord.X * OriginalTileSize.X)) / CurrentTileSize.X);

        //Если по какой-либо причине минимальная коордиата смещения от зума стала больше 0, то это исправляется. Зум никогда не приблизит контент ближе изначальной нулевой позиции
        if (MinZoomCoord.X > 0)
            MinZoomCoord.X = 0;

        /* Производится проверка не привысило ли количество отображаемых тайлов реальное количество тайлов. Да, такое вполне может произойти
         * в моменты прокрутки контента близко от края родительского виджета из-за большого количества float'ов и неточного, но быстрого способа
         * рассчёта длины части контента, торчащей из-за пределов своей области. Если же количество тайлов для отображения действительно вышло
         * за пределы реального количества тайлов, то, зная, что такая проблема может возникнуть лишь почти вплотную у края виджета, просто
         * делаются видимыми все тайлы от габаритов смещения до соответствующей стороны контента. Тем самым лишняя часть габаритов зума просто
         * усекается до реальных габаритов контента. */
        if (Bias.X > 0 && MaxZoomCoord.X + MaxBiasCoord.X > SizeDifference.X / OriginalTileSize.X / 2.0)
            MaxZoomCoord.X = SizeDifference.X / OriginalTileSize.Y / 2.0 - MaxBiasCoord.X;
        if (Bias.X < 0 && MinZoomCoord.X + MinBiasCoord.X < -SizeDifference.X / OriginalTileSize.X / 2.0)
            MinZoomCoord.X = -(SizeDifference.X / OriginalTileSize.Y / 2.0 + MinBiasCoord.X);

        /* расчёты появления новых отображаемых тайлов от зума у координаты Y идентичны расчётам по координате X, где
         * MaxZoomCoord.Y - верх, который считается как право, а MinZoomCoord.Y - низ, который считается как лево */

        MaxZoomCoord.Y = ceil((SizeDifferenceBetweenOriginalAndCurrentContentSize.Y - DistanceToAppearanceOfFirstNewTile.Y * ZoomMultiplier +
            (Bias.Y - MaxBiasCoord.Y * OriginalTileSize.Y)) / CurrentTileSize.Y);

        if (MaxZoomCoord.Y < 0)
            MaxZoomCoord.Y = 0;

        MinZoomCoord.Y = -ceil((SizeDifferenceBetweenOriginalAndCurrentContentSize.Y - DistanceToAppearanceOfFirstNewTile.Y * ZoomMultiplier -
            (Bias.Y - MinBiasCoord.Y * OriginalTileSize.Y)) / CurrentTileSize.Y);

        if (MinZoomCoord.Y > 0)
            MinZoomCoord.Y = 0;

        if (Bias.Y > 0 && MaxZoomCoord.Y + MaxBiasCoord.Y > SizeDifference.Y / OriginalTileSize.Y / 2.0)
            MaxZoomCoord.Y = SizeDifference.Y / OriginalTileSize.Y / 2.0 - MaxBiasCoord.Y;
        if (Bias.Y < 0 && MinZoomCoord.Y + MinBiasCoord.Y < -SizeDifference.Y / OriginalTileSize.Y / 2.0)
            MinZoomCoord.Y = -(SizeDifference.Y / OriginalTileSize.Y / 2.0 + MinBiasCoord.Y);

        //Координаты тайлов, отражающие масштабирование контента, записываются в габариты разницы между изначальным размером контента и его текущим размером
        ZoomDimentions = FDimensionsDisplayedArea(MinZoomCoord, MaxZoomCoord);
    }

    CurrentDimensions = CurrentDimensions + ZoomDimentions;

    /* Бывают моменты, когда размер контента меньше минимального. В этом случае нужные размеры виджету контента помогает
     * держать специальная подложка, но размер меньше минимального так же говорит и о том, что все рассчёты по габаритам
     * были не нужны, а настоящая рабочая область контента банально меньше размера своего виджета, так что по соответствующей
     * координате все тайлы должны быть всегда показаны */
    bool CurrentXLessThanMin = OriginalTableSize.X < minContentSize.X;
    bool CurrentYLessThanMin = OriginalTableSize.Y < minContentSize.Y;

    if (TilesCoordWrapper->HasAnyEllements()) {
        //Если одна из осей контента меньше минимального размера, то отображаются все тайлы по соответствующей оси
        if (CurrentXLessThanMin) {
            CurrentDimensions.Min.X = 0;
            CurrentDimensions.Max.X = TableTileCols - 1;
        }

        if (CurrentYLessThanMin) {
            CurrentDimensions.Min.Y = 0;
            CurrentDimensions.Max.Y = TableTileRows - 1;
        }

        //Изменения производятся если габариты контента изменились
        if (OldDimensions != CurrentDimensions) {
            //Чтобы таргитировано убирать или добавлять строки и стобцы, проверяется как именно изменились габариты контента
            bool NewTopBoundMore = (CurrentDimensions.Max.Y > OldDimensions.Max.Y);
            bool OldTopBoundLess = CurrentDimensions.Max.Y < OldDimensions.Max.Y;
            bool NewBottomBoundMore = CurrentDimensions.Min.Y < OldDimensions.Min.Y;
            bool OldBottomBoundLess = CurrentDimensions.Min.Y > OldDimensions.Min.Y;
            bool NewLeftBoundMore = CurrentDimensions.Min.X < OldDimensions.Min.X;
            bool OldLeftBoundLess = CurrentDimensions.Min.X > OldDimensions.Min.X;
            bool NewRightBoundMore = CurrentDimensions.Max.X > OldDimensions.Max.X;
            bool OldRightBoundLess = CurrentDimensions.Max.X < OldDimensions.Max.X;

            if (!CurrentYLessThanMin) {
                if (NewTopBoundMore) {
                    for (int row = OldDimensions.Max.Y + 1; row <= CurrentDimensions.Max.Y; row++) {
                        for (int col = CurrentDimensions.Min.X; col <= CurrentDimensions.Max.X; col++) {
                            UWidget* GridPanelElement = TilesCoordWrapper->FindWidget(row, col);
                            if (GridPanelElement) {
                                GridPanelElement->SetVisibility(ESlateVisibility::Visible);
                            }
                            else
                                UE_LOG(LogTemp, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: When trying to display a new top tile at coordinates row: %d, col: %d, an error occurred - the GridPanelElement is not valid"), row, col);
                        }
                    }
                }

                if (OldTopBoundLess) {
                    for (int row = CurrentDimensions.Max.Y + 1; row <= OldDimensions.Max.Y; row++) {
                        for (int col = OldDimensions.Min.X; col <= OldDimensions.Max.X; col++) {
                            UWidget* GridPanelElement = TilesCoordWrapper->FindWidget(row, col);
                            if (GridPanelElement) {
                                GridPanelElement->SetVisibility(ESlateVisibility::Collapsed);
                            }
                            else
                                UE_LOG(LogTemp, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: When trying to hide a top tile at coordinates row: %d, col: %d, an error occurred - the GridPanelElement is not valid"), row, col);
                        }
                    }
                }

                if (NewBottomBoundMore) {
                    for (int row = CurrentDimensions.Min.Y; row <= OldDimensions.Min.Y - 1; row++) {
                        for (int col = CurrentDimensions.Min.X; col <= CurrentDimensions.Max.X; col++) {
                            UWidget* GridPanelElement = TilesCoordWrapper->FindWidget(row, col);
                            if (GridPanelElement) {
                                GridPanelElement->SetVisibility(ESlateVisibility::Visible);
                            }
                            else
                                UE_LOG(LogTemp, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: When trying to display a new bottom tile at coordinates row: %d, col: %d, an error occurred - the GridPanelElement is not valid"), row, col);
                        }
                    }
                }

                if (OldBottomBoundLess) {
                    for (int row = OldDimensions.Min.Y; row <= CurrentDimensions.Min.Y - 1; row++) {
                        for (int col = OldDimensions.Min.X; col <= OldDimensions.Max.X; col++) {
                            UWidget* GridPanelElement = TilesCoordWrapper->FindWidget(row, col);
                            if (GridPanelElement) {
                                GridPanelElement->SetVisibility(ESlateVisibility::Collapsed);
                            }
                            else
                                UE_LOG(LogTemp, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: When trying to hide a bottom tile at coordinates row: %d, col: %d, an error occurred - the GridPanelElement is not valid"), row, col);
                        }
                    }
                }
            }

            if (!CurrentXLessThanMin) {
                if (NewLeftBoundMore) {
                    for (int row = CurrentDimensions.Min.Y; row <= CurrentDimensions.Max.Y; row++) {
                        for (int col = CurrentDimensions.Min.X; col <= OldDimensions.Min.X - 1; col++) {
                            UWidget* GridPanelElement = TilesCoordWrapper->FindWidget(row, col);
                            if (GridPanelElement) {
                                GridPanelElement->SetVisibility(ESlateVisibility::Visible);
                            }
                            else
                                UE_LOG(LogTemp, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: When trying to display a new left tile at coordinates row: %d, col: %d, an error occurred - the GridPanelElement is not valid"), row, col);
                        }
                    }
                }

                if (OldLeftBoundLess) {
                    for (int row = OldDimensions.Min.Y; row <= OldDimensions.Max.Y; row++) {
                        for (int col = OldDimensions.Min.X; col <= CurrentDimensions.Min.X - 1; col++) {
                            UWidget* GridPanelElement = TilesCoordWrapper->FindWidget(row, col);
                            if (GridPanelElement) {
                                GridPanelElement->SetVisibility(ESlateVisibility::Collapsed);
                            }
                            else
                                UE_LOG(LogTemp, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: When trying to hide a left tile at coordinates row: %d, col: %d, an error occurred - the GridPanelElement is not valid"), row, col);
                        }
                    }
                }

                if (NewRightBoundMore) {
                    for (int row = CurrentDimensions.Min.Y; row <= CurrentDimensions.Max.Y; row++) {
                        for (int col = OldDimensions.Max.X + 1; col <= CurrentDimensions.Max.X; col++) {
                            UWidget* GridPanelElement = TilesCoordWrapper->FindWidget(row, col);
                            if (GridPanelElement) {
                                GridPanelElement->SetVisibility(ESlateVisibility::Visible);
                            }
                            else
                                UE_LOG(LogTemp, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: When trying to display a new right tile at coordinates row: %d, col: %d, an error occurred - the GridPanelElement is not valid"), row, col);
                        }
                    }
                }

                if (OldRightBoundLess) {
                    for (int row = OldDimensions.Min.Y; row <= OldDimensions.Max.Y; row++) {
                        for (int col = CurrentDimensions.Max.X + 1; col <= OldDimensions.Max.X; col++) {
                            UWidget* GridPanelElement = TilesCoordWrapper->FindWidget(row, col);
                            if (GridPanelElement) {
                                GridPanelElement->SetVisibility(ESlateVisibility::Collapsed);
                            }
                            else
                                UE_LOG(LogTemp, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: When trying to hide a right tile at coordinates row: %d, col: %d, an error occurred - the GridPanelElement is not valid"), row, col);
                        }
                    }
                }
            }
        }

        GridCoord MinCoord = TilesCoordWrapper->getMinCoord();
        if (MinCoord.getIsInit()) {
            UWidget* HiddenWidget = TilesCoordWrapper->FindWidget(MinCoord.Row, MinCoord.Col);
            if (HiddenWidget) {
                if (HiddenWidget->GetVisibility() == ESlateVisibility::Collapsed)
                    HiddenWidget->SetVisibility(ESlateVisibility::Hidden);
            }
            else
                UE_LOG(LogTemp, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: A tile found at the TilesCoordWrapper minimum coordinate is not valid"));

        }else
            UE_LOG(LogTemp, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: TilesCoordWrapper minimum coordinate is not initialized"));

        GridCoord MaxCoord = TilesCoordWrapper->getMaxCoord();
        if (MaxCoord.getIsInit()) {
            UWidget* HiddenWidget = TilesCoordWrapper->FindWidget(MaxCoord.Row, MaxCoord.Col);
            if (HiddenWidget) {
                if (HiddenWidget->GetVisibility() == ESlateVisibility::Collapsed)
                    HiddenWidget->SetVisibility(ESlateVisibility::Hidden);
            }
            else
                UE_LOG(LogTemp, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: A tile found at the TilesCoordWrapper maximum coordinate is not valid"));
        }
        else
            UE_LOG(LogTemp, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: TilesCoordWrapper maximum coordinate is not initialized"));

    }
    else
        UE_LOG(LogTemp, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: TilesCoordWrapper does not include any elements"));

    OldDimensions = CurrentDimensions;
}

/* Функция, скрывающая отображаемые в данный момент тайлы, делая всю таблицу полностью
 * неактивной. Используется для сброса состояния таблицы перед переинициализацией */
void UTileTablesOptimizationTools::ExtinguishCurrentDimension(UCoordWrapperOfTable* TilesCoordWrapper)
{
    for (int row = CurrentDimensions.Min.Y; row <= CurrentDimensions.Max.Y; row++) {
        for (int col = CurrentDimensions.Min.X; col <= CurrentDimensions.Max.X; col++) {
            UWidget* GridPanelElement = TilesCoordWrapper->FindWidget(row, col);
            if (GridPanelElement)
                GridPanelElement->SetVisibility(ESlateVisibility::Collapsed);
            else
                UE_LOG(LogTemp, Error, TEXT("CHORT"));
        }
    }

    OldDimensions = FDimensionsDisplayedArea();
    CurrentDimensions = FDimensionsDisplayedArea();
}

FTileCoord::FTileCoord(int32 x, int32 y) : X(x), Y(y)
{}

//Преобразоывание координаты в текст для вывода в логи
FString FTileCoord::ToString()
{
    return FString("X: " + FString::FromInt(X) + " Y: " + FString::FromInt(Y));
}

bool FTileCoord::operator==(const FTileCoord& Coord) const
{
    return X == Coord.X && Y == Coord.Y;
}

bool FTileCoord::operator!=(const FTileCoord& Coord) const
{
    return X != Coord.X || Y != Coord.Y;
}

FTileCoord FTileCoord::operator+(const FTileCoord Bias) const
{
    return FTileCoord(X + Bias.X, Y + Bias.Y);
}

FTileCoord FTileCoord::operator-(const FTileCoord Bias) const
{
    return FTileCoord(X - Bias.X, Y - Bias.Y);
}

FDimensionsDisplayedArea::FDimensionsDisplayedArea(FTileCoord min, FTileCoord max) : Min(min), Max(max)
{}

FString FDimensionsDisplayedArea::ToString()
{
    return FString("Min(" + Min.ToString() + "), Max(" + Max.ToString() + ")");
}

bool FDimensionsDisplayedArea::IsEmpty()
{
    return Min.X == 0 && Min.Y == 0 && Max.X == 0 && Max.Y == 0;
}

bool FDimensionsDisplayedArea::operator==(const FDimensionsDisplayedArea& Dimensions) const
{
    return Min == Dimensions.Min && Max == Dimensions.Max;
}

bool FDimensionsDisplayedArea::operator!=(const FDimensionsDisplayedArea& Dimensions) const
{
    return Min != Dimensions.Min || Max != Dimensions.Max;
}

FDimensionsDisplayedArea FDimensionsDisplayedArea::operator+(const FDimensionsDisplayedArea Bias) const
{
    return FDimensionsDisplayedArea(Min + Bias.Min, Max + Bias.Max);
}

FDimensionsDisplayedArea FDimensionsDisplayedArea::operator-(const FDimensionsDisplayedArea Bias) const
{
    return FDimensionsDisplayedArea(Min - Bias.Min, Max - Bias.Max);
}