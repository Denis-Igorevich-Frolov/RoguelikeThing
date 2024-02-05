// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Function Libraries/TileTablesOptimizationTools.h"
#include <cmath>

DEFINE_LOG_CATEGORY(TileTablesOptimizationTools);
#pragma optimize( "", off )

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
            int NumberOfCollapsedTilesTopAndBottom = (SizeDifference.Y / 2) / OriginalTileSize.Y;
            int NumberOfCollapsedTilesRinhtAndLeft = (SizeDifference.X / 2) / OriginalTileSize.X;

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
            DistanceToAppearanceOfFirstNewTile = (OriginalDimensionsSize - widgetAreaSize) / 2;
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
        UE_LOG(TileTablesOptimizationTools, Error, TEXT("!!! An error occurred in the ChangingVisibilityOfTableTiles class in the InitTableTiles function: TilesCoordWrapper is not valid"));
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
        if (Bias.X < SizeDifference.X / 2) {
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
            MinBiasCoord.X = MaxBiasCoord.X = SizeDifference.X / 2 / OriginalTileSize.X;
    }
    //Иначе сдвиг видимой области влево
    else {
        //Если камера не уткнулась в край контента, то все изменения происходят в обычном режиме
        if (Bias.X > -(SizeDifference.X / 2)) {
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
            MinBiasCoord.X = MaxBiasCoord.X = -SizeDifference.X / 2 / OriginalTileSize.X;
    }

    //Смещение по Y больше 0 означает сдвиг видимой области вверх
    if (Bias.Y > 0) {
        //Если камера не уткнулась в край контента, то все изменения происходят в обычном режиме
        if (Bias.Y < SizeDifference.Y / 2) {
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
            MinBiasCoord.Y = MaxBiasCoord.Y = SizeDifference.Y / 2 / OriginalTileSize.Y;
    }
    //Иначе сдвиг видимой области вниз
    else {
        //Если камера не уткнулась в край контента, то все изменения происходят в обычном режиме
        if (Bias.Y > -(SizeDifference.Y / 2)) {
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
            MinBiasCoord.Y = MaxBiasCoord.Y = -SizeDifference.Y / 2 / OriginalTileSize.Y;
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
        FVector2D CurrentDimensionsSize = FVector2D((OriginalDimensions.Max.X - OriginalDimensions.Min.X + 1) * CurrentTileSize.X,
            (OriginalDimensions.Max.Y - OriginalDimensions.Min.Y + 1) * CurrentTileSize.Y);

        /*!!!!!!(предварительный быстрый комментарий) : (OriginalDimensionsSize.X - CurrentDimensionsSize.X) - разница между изначальным размером контента и текущим,
        она делится по полам потому что сторона одна. Это база, которую надо забить тайлами, но совпадение с экраном же не сто процентное, и
        DistanceToAppearanceOfFirstNewTile - это как раз то, на сколько контент торчит за пределами своего окна. Оговорка - DistanceToAppearanceOfFirstNewTile,
        в отлицие от всего остального в вычислении, зависит от изначального размера контента, а не текущего, так как контент уменьшается, а окно просмотра - нет, а
        так как следует забить разницу между изначальным и текущим размером, следует брать "торчание" именно базового размера. Так вот, из этой базы вычитается
        размер изначального торчания, чтобы отдалить начало заполнения расстояния ячейками на то расстояние, которое изначально торчало. И это торчание должно
        компенчироваться либо всё большим масштабированием, либо сдвигом. Но весь сдвиг не нужен, так как появление ячеек от сдвига задаётся не здесь. А нужен
        лишь остаток от деления сдвига на текущий размер тайла. Этот "лишний" размер как раз тот размер, на который можно сдвинуть контент, чтобы не появилась ячейка
        от сдвига, но уже могла бы появиться дополнительная ячейка от масштабирования. Этот размер следует прибавить к базе, чтобы приблизить появление нового тайла.
        Ну или вычесть, если Bias.X < 0. Это как бы тогда я прибавляю модуль сдвига в зависимости от стороны сдвига. Потом это очевидно просто делится на актуальную
        длину тайла и всё округляется вверх, так как даже небольшого размера, попавшего в область видимости, достаточно, чтобы появилась необходимость отрисовать весь тайл.*/
        if(Bias.X > 0)
            MinZoomCoord.X = -ceil(((OriginalDimensionsSize.X - CurrentDimensionsSize.X) / 2 - DistanceToAppearanceOfFirstNewTile.X + fmod(Bias.X, CurrentTileSize.X)) / CurrentTileSize.X);
        else
            MinZoomCoord.X = -ceil(((OriginalDimensionsSize.X - CurrentDimensionsSize.X) / 2 - DistanceToAppearanceOfFirstNewTile.X - fmod(Bias.X, CurrentTileSize.X)) / CurrentTileSize.X);
        //if (MinZoomCoord.X < 0)
        //    MinZoomCoord.X = 0;

        MaxZoomCoord.X = 0;
         if (Bias.X > 0 && MaxZoomCoord.X + MaxBiasCoord.X > SizeDifference.X / OriginalTileSize.X / 2)
            MaxZoomCoord.X = SizeDifference.X / OriginalTileSize.Y / 2 - MaxBiasCoord.X;
        if (Bias.X < 0 && MinZoomCoord.X + MinBiasCoord.X < -SizeDifference.X / OriginalTileSize.X / 2)
            MinZoomCoord.X = -(SizeDifference.X / OriginalTileSize.Y / 2 + MinBiasCoord.X);

        float Y_SizeDifferenceWhenZoom = widgetAreaSize.Y - (OriginalDimensionsSize.Y * ZoomMultiplier);
        if (Y_SizeDifferenceWhenZoom > 0)
            MaxZoomCoord.Y = 0;
        else
            Y_SizeDifferenceWhenZoom = 0;
        MinZoomCoord.Y = -MaxZoomCoord.Y;
        if (Bias.Y > 0 && MaxZoomCoord.Y + MaxBiasCoord.Y > SizeDifference.Y / OriginalTileSize.Y / 2)
            MaxZoomCoord.Y = SizeDifference.Y / OriginalTileSize.Y / 2 - MaxBiasCoord.Y;
        if (Bias.Y < 0 && MinZoomCoord.Y + MinBiasCoord.Y < -SizeDifference.Y / OriginalTileSize.Y / 2)
            MinZoomCoord.Y = -(SizeDifference.Y / OriginalTileSize.Y / 2 + MinBiasCoord.Y);

        ZoomDimentions = FDimensionsDisplayedArea(MinZoomCoord, MaxZoomCoord);
    }

    CurrentDimensions = CurrentDimensions + ZoomDimentions;

    bool CurrentXLessThanMin = OriginalTableSize.X < minContentSize.X;
    bool CurrentYLessThanMin = OriginalTableSize.Y < minContentSize.Y;

    if (TilesCoordWrapper->HasAnyEllements()) {
        if (CurrentXLessThanMin) {
            CurrentDimensions.Min.X = 0;
            CurrentDimensions.Max.X = TableTileCols - 1;
        }

        if (CurrentYLessThanMin) {
            CurrentDimensions.Min.Y = 0;
            CurrentDimensions.Max.Y = TableTileRows - 1;
        }

        if (OldDimensions != CurrentDimensions) {
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
                                UE_LOG(LogTemp, Error, TEXT("NewTopBoundMore CHORT"));
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
                                UE_LOG(LogTemp, Error, TEXT("OldTopBoundLess CHORT"));
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
                                UE_LOG(LogTemp, Error, TEXT("NewBottomBoundMore CHORT"));
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
                                UE_LOG(LogTemp, Error, TEXT("OldBottomBoundLess CHORT"));
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
                                UE_LOG(LogTemp, Error, TEXT("NewLeftBoundMore CHORT"));
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
                                UE_LOG(LogTemp, Error, TEXT("OldLeftBoundLess CHORT"));
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
                                UE_LOG(LogTemp, Error, TEXT("NewRightBoundMore CHORT"));
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
                                UE_LOG(LogTemp, Error, TEXT("OldRightBoundLess CHORT"));
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
                UE_LOG(LogTemp, Error, TEXT("MinCoord CHORT 1"));

        }else
            UE_LOG(LogTemp, Error, TEXT("MinCoord CHORT 2"));

        GridCoord MaxCoord = TilesCoordWrapper->getMaxCoord();
        if (MaxCoord.getIsInit()) {
            UWidget* HiddenWidget = TilesCoordWrapper->FindWidget(MaxCoord.Row, MaxCoord.Col);
            if (HiddenWidget) {
                if (HiddenWidget->GetVisibility() == ESlateVisibility::Collapsed)
                    HiddenWidget->SetVisibility(ESlateVisibility::Hidden);
            }
            else
                UE_LOG(LogTemp, Error, TEXT("MaxCoord CHORT 1"));
        }
        else
            UE_LOG(LogTemp, Error, TEXT("MaxCoord CHORT 2"));

    }

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