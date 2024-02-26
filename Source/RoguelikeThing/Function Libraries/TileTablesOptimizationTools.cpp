// Denis Igorevich Frolov did all this. Once there. All things reserved.

#include "RoguelikeThing/Function Libraries/TileTablesOptimizationTools.h"

DEFINE_LOG_CATEGORY(TileTablesOptimizationTools);

/* ��������� ������������� ������� ������ ����� �������, ����� � ��� ��������
 * ���� ����� �� �����, ������� ������ � ������� ������������� ������� */
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
            //���������� ������ � ������� ������� ��������� ����������� �������� ����� ������� ��������� �� ����� �����
            int TilesInFragmentLen = MapDimensions.TableLength / TileLen;

            /* ��� ���������� ���������� ����� ������ � �������� ������� ������� ��������� ���������� ����� ����������.
             * ��� ��� ����������� ���������� ���������� �� ����������� ������ ���� 0:0, ����� ���������� ����� ���������
             * ���� ��������� �� ���������� ���������� ����������. �� ��� ��� ���������� ������������� � ���������, �
             * ������� ������ ������ � 0, �� ���������� �������� ������� �������� � ����������� ������ �������� �������.
             * ����� ���������� ���������� ����� ���������� ������ ���������� �� ���������� ������ � ������� ������� ���������� */
            TableTileRows = (MapDimensions.MaxRow - MapDimensions.MinRow + 1) * TilesInFragmentLen;
            //���������� �������� ������ �����, ��� � ���������� �����
            TableTileCols = (MapDimensions.MaxCol - MapDimensions.MinCol + 1) * TilesInFragmentLen;

            //������������ �������� ���������� ����� � ��������, ��� ��� ������������ ���������� ����� �� ���������
            RealTableTileRows = TableTileRows;
            RealTableTileCols = TableTileCols;

            //������� �� ���������� ����� 3 ���������� � ������� ��� �������� �� ���. ��� ������� ��� �����������
            if (TableTileRows > 3 * TilesInFragmentLen)
                TableTileRows = 3 * TilesInFragmentLen;
            if (TableTileCols > 3 * TilesInFragmentLen)
                TableTileCols = 3 * TilesInFragmentLen;

            /* ������������ �� �������, �������� ���������� �������� ������� ����� � �������
             * ��������. ��� ����������� ����� ��� ���������� ��������������� � ������ */
            OriginalTileSize = TileSize;
            widgetAreaSize = WidgetAreaSize;

            //������� ���������� ������� ����� ��������� ������� ������� ����� �� ���������� �������� � �����
            OriginalTableSize = OriginalTileSize * FVector2D(TableTileCols, TableTileRows);
            //����������� ������� � �������� ����� �������� �������� � �����, � ������� ��� ���������
            SizeDifference = OriginalTableSize - widgetAreaSize;

            /* ���� ������ �������� ������� ������, ��� ����������� ������, �������������� ��� ������� �������
             * ��������, �� ������� �������� ������������, ����� ���������� ��������� ������ ������������� ������� */
            if (OriginalTableSize.X < minContentSize.X)
                SizeDifference.X = 0;
            if (OriginalTableSize.Y < minContentSize.Y)
                SizeDifference.Y = 0;

            //�� ������ ������� �������� ����������� ������� ����� � � �������� ������ ���� ������ ������ � �����, ������ � �����
            int NumberOfCollapsedTilesTopAndBottom = (SizeDifference.Y / 2.0) / OriginalTileSize.Y;
            int NumberOfCollapsedTilesRinhtAndLeft = (SizeDifference.X / 2.0) / OriginalTileSize.X;

            /* ���� �� �����-�� �������� ����� ������� �������� ��� ����� �������������, ��
             * ���� �������� ������������. ������������� �������� ����� ������ ������ ������� */
            if (NumberOfCollapsedTilesTopAndBottom < 0)
                NumberOfCollapsedTilesTopAndBottom = 0;
            if (NumberOfCollapsedTilesRinhtAndLeft < 0)
                NumberOfCollapsedTilesRinhtAndLeft = 0;

            /***************************************************************/UE_LOG(LogTemp, Log, TEXT("%d, %d"), NumberOfCollapsedTilesTopAndBottom, NumberOfCollapsedTilesRinhtAndLeft);//!!!!

            /* ������������ ������� �������� ������������ �������. ����������� ����� ����� ����� �� ����� �����
             * ������ ������� ����� � ��������, � ��� ��� ������� ����� - ��� ������, ���������� ����� �����
             * �������� � ���� ���������� �������. �� ������ ���������� ����� ����������, ��� ��� ������ ���
             * ����� ������� ������, � �� �� ���, �� ���� � ������ ����� ����������� ��� � ��������� �������.
             * � ������������ �� ����� ��������� ������� ����������, ��� ��� ��� ���������� ��������� � ������
             * ������� ����� �������, �� ���� "����� ����� ������� ����� � ��������" ��� ���� ����� ����� ��
             * ���� ���� ����� ����� ������������ ����� ������� ������, � �� ������ ������, ��� � ����������� ����� */
            CurrentDimensions = FDimensionsDisplayedArea(
                FTileCoord(NumberOfCollapsedTilesRinhtAndLeft, NumberOfCollapsedTilesTopAndBottom),
                FTileCoord(TableTileCols - NumberOfCollapsedTilesRinhtAndLeft - 1, TableTileRows - NumberOfCollapsedTilesTopAndBottom - 1));

            /***************************************************************/UE_LOG(LogTemp, Log, TEXT("CurrentDimensions: %s"), *CurrentDimensions.ToString());//!!!!

            //��� �����, �������� � ������� ������� ��������� �������� ��������
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

            /* ���������� ��� ������ Collapsed ����� ������ ������ �������� ����������, ��� ������� � ������� Hidden,
             * ��� ��� �� ��������� ��� �����, ����� ������� � ������ ������ ���������. �� ��-�� ����� �������� �������
             * ��������� �� ��������� ������� �������, � ���� ����� ������ ������ �������, �� ����� ������� �������
             * ������ ���������� �������������� �� ���� � �������. ������ ��� ����� ������ - ������� ������� ��������
             * (Hidden), � �� ����������� (Collapsed) ����� � ���������� � ���������� ������������. ��� ����� ��� ��
             * "����������" ��� �������, �������� � � ���� ��������������� ����� */

            GridCoord MinCoord = TilesCoordWrapper->getMinCoord();
            if (MinCoord.getIsInit()) {
                UWidget* HiddenWidget = TilesCoordWrapper->FindWidget(MinCoord.Row, MinCoord.Col);
                /* ���� ������� ���� �� ��������, ��� ������, ��� ������ ���� ������� �� ��������� �������� ������� 
                 * ������, � ������������� ���� ���� �����. � ����� ������ ��������� � "���������" ������� ��� */
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
                /* ���� ������� ���� �� ��������, ��� ������, ��� ������ ���� ������� �� ��������� �������� �������
                 * ������, � ������������� ���� ���� �����. � ����� ������ ��������� � "���������" ������� ��� */
                if (HiddenWidget && HiddenWidget->GetVisibility() == ESlateVisibility::Collapsed)
                    HiddenWidget->SetVisibility(ESlateVisibility::Hidden);
            }
            else {
                UE_LOG(TileTablesOptimizationTools, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the InitTableTiles function: Max coord tile Row:%d Col:%d is not initialized"), MaxCoord.Row, MaxCoord.Col);
                InitializationValidity = FInitializationValidity::ERROR;
                return FVector2D();
            }

            //�� ������ ����� ������� �������� ������� ������� � ���� ����������� ��������
            OriginalDimensions = CurrentDimensions;
            //��������, ������������ ������ ������� ������� �� ���������� ������������ ������� ���������� ���������������� ���������� ����������
            OldDimensions = OriginalDimensions;

            //�������� �������, ���������� � Vector2D, ��� ��������� ���������� ������� �������. ����������� �������� - ��� ���������� ������� � ����������� ������
            OriginalDimensionsSize = FVector2D((OriginalDimensions.Max.X - OriginalDimensions.Min.X + 1) * OriginalTileSize.X,
                (OriginalDimensions.Max.Y - OriginalDimensions.Min.Y + 1) * OriginalTileSize.Y);

            /* ����������, ��� ����������� �������� ���������� ������ ����� ���� ����������� ���� ���������� �������� �������
             * ����� ��������� ���������� ������� ������� � ������� ������������� ��������. ��� ����� ��������� ������, ������
             * ������ �������, ������� ��� �� ������ �� ��������� �������, ������ �������� �������� �������� ������� */
            DistanceToAppearanceOfFirstNewTile = (OriginalDimensionsSize - widgetAreaSize) / 2.0;
            /* ���� ������ �������� ������� ������, ��� ������, � ������� ��� ��������, �� ���������� ���������� �� ���������
             * ������ ������� ����� ����� ����� �������������. � ����� ������, ������� ����� ������ �� ��������� ������� ���, �
             * ���������� ������. ��������� �������������� � 0, ��� ����� ����������� ����, ��� ������ ������� �� ���� */
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

//�������, ���������� ��������� ������ �� ������ ��� ��������������� �������
void UTileTablesOptimizationTools::ChangingVisibilityOfTableTiles(UCoordWrapperOfTable* TilesCoordWrapper, FVector2D Bias, float ZoomMultiplier)
{
    //����������� ���������� ���������� ������������� �������
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

    /* ��� �������������� �������� ����� ���� ������������ ����� ������� ����� ����������� �������� �������
     * � ������ ������� � ��� ������� ��������, ��� ������ ������� �� ���������� X ��� �������������� ������
     * ������ �������������. ��� � �������, �� ��� ����� ������������ �� ����� �������� � ����� ������� ������,
     * � ��� �� "����� ������" �� ���� ������� �������� ������, ������� ���������� ��������������� */
    Bias.X *= -1;

    //����������� ������, ���������� ����� ��������, ������� ����� ���������� � ����������� ����������� ��� ���������� �������� ������� ��������
    FTileCoord MinBiasCoord;
    FTileCoord MaxBiasCoord;

    //�������� �� X ������ 0 �������� ����� ������� ������� ������
    if (Bias.X > 0) {
        //���� ������ �� ��������� � ���� ��������, �� ��� ��������� ���������� � ������� ������
        if (Bias.X < SizeDifference.X / 2.0) {
            /* �������� �����������, � ���������� �����, ���������� ��� ������ ������ ����� ������� ������ �������� �� ��������� ������ ����� �����.
             * ���������� �� ��������� ������� ������ ����� ����� ������������ � ������ ��������, ����� ������, ��� ������ ���� ����� �������� ������
             * ���������, � ������ � ������ ������ ����� �������� � ���������� �� ��������� ������ ����� ������ ������ ���� �� 1 ������ �����. �� ����
             * �������� ���������� ����� ������� ����������, ������ ������� ����� ������� ����� � ����������� �� ��������� ������� �����, ����� �������
             * ����� ���� ������ ���� */
            MinBiasCoord.X = (Bias.X + DistanceToAppearanceOfFirstNewTile.X) / OriginalTileSize.X;
            /* �������� ������������, � ���������� ������, ���������� ��� ������ ������ ����� ������� ������ �������� �� ����������� ����� ������ �����.
             * ������� ������ ����� � ���������� �� ��������� ������� ������ ����� ����� ������������ � ������ ��������, ����� ������, ��� ������ ����
             * ����� ���������� ������ ���������, � ������ � ������ ������ ����� �������� � ������� �� ������ ����� ������ ������ ���� �� 1 ������ �����.
             * �� ���� �������� ���������� ����� ������� ����������, ������ ���������� �� ��������� ������� �����, ����� ���������� ������ ���� ������ ���� */
            MaxBiasCoord.X = (Bias.X + (OriginalTileSize.X - DistanceToAppearanceOfFirstNewTile.X)) / OriginalTileSize.X;
        }
        else
            /* ��� ������, ���������� � ������� ��������, ��������� �������� ��������� ����� - ��� ������ ����� ����� ���� ����� � ������
             * ���������� ������, ����� �������� � ���������� �� ������� ������� ���������� ������ ������ �� ��������� ����� ������� */
            MinBiasCoord.X = MaxBiasCoord.X = SizeDifference.X / 2.0 / OriginalTileSize.X;
    }
    //����� ����� ������� ������� �����
    else {
        //���� ������ �� ��������� � ���� ��������, �� ��� ��������� ���������� � ������� ������
        if (Bias.X > -(SizeDifference.X / 2.0)) {
            /* �������� ������������, � ���������� ������, ���������� ��� ������ ������ ������� ������ ������ �������� �� �������� ������ ������ �����.
             * ���������� �� ��������� ������� ������ ����� ����� ���������� �� ������ ��������, ������� ����� ������ ������� ��� �������������, �����
             * ������, ��� ������ ���� ����� ������� ������ ���������, � ������ � ������ ������ ������ ������� �������� � ���������� �� ��������� ������
             * ����� ������� ������ ���� �� 1 ������ �����. �� ���� �������� ���������� ����� ������� ����������, ������ ������ ������� ����� �������
             * ����� � ����������� �� ��������� ������� �����, ����� ������� ������ ���� ������ ���� */
            MaxBiasCoord.X = (Bias.X - DistanceToAppearanceOfFirstNewTile.X) / OriginalTileSize.X;
            /* �������� �����������, � ���������� �����, ���������� ��� ������ ������ ������� ������ ������ �������� �� ����������� ����� ����� �����.
             * ������� ������ ����� � ���������� �� ��������� ������� ������ ����� ����� ���������� �� ������ ��������, ������� ����� ������ �������
             * ��� �������������, ����� ������, ��� ������ ���� ����� ���������� ������ ���������, � ������ � ������ ������ ������ ������� �������� �
             * ������� �� ������ ����� ������� ������ ���� �� 1 ������ �����. �� ���� �������� ���������� ����� ������� ����������, ������ ������
             * ���������� �� ��������� ������� �����, ����� ���������� ����� ���� ������ ���� */
            MinBiasCoord.X = (Bias.X - (OriginalTileSize.X - DistanceToAppearanceOfFirstNewTile.X)) / OriginalTileSize.X;
        }
        else
            /* ��� ������, ���������� � ������� ��������, ��������� �������� ��������� ����� - ��� ������ ����� ����� ���� ����� � ������
             * ���������� ������, ����� �������� � ������ ���������� �� ������� ������� ���������� ������ ����� �� ��������� ����� ������� */
            MinBiasCoord.X = MaxBiasCoord.X = -SizeDifference.X / 2.0 / OriginalTileSize.X;
    }

    //�������� �� Y ������ 0 �������� ����� ������� ������� �����
    if (Bias.Y > 0) {
        //���� ������ �� ��������� � ���� ��������, �� ��� ��������� ���������� � ������� ������
        if (Bias.Y < SizeDifference.Y / 2.0) {
            /* �������� �����������, � ���������� ������, ���������� ��� ������ ������ ����� ����� ������ �������� �� ��������� ������ ������ �����.
             * ���������� �� ��������� ������� ������ ����� ����� ������������ � ������ ��������, ����� ������, ��� ������ ���� ����� �������� ������
             * ���������, � ������ � ������ ������ ����� �������� � ���������� �� ��������� ������ ����� ������ ������ ���� �� 1 ������ �����. �� ����
             * �������� ���������� ����� ������� ����������, ������ ������� ����� ������� ����� � ����������� �� ��������� ������� �����, ����� �������
             * ����� ���� ������ ���� */
            MinBiasCoord.Y = (Bias.Y + DistanceToAppearanceOfFirstNewTile.Y) / OriginalTileSize.Y;
            /* �������� ������������, � ���������� �������, ���������� ��� ������ ������ ����� ����� ������ �������� �� ����������� ����� ������� �����.
             * ������� ������ ����� � ���������� �� ��������� ������� ������ ����� ����� ������������ � ������ ��������, ����� ������, ��� ������ ����
             * ����� ���������� ������ ���������, � ������ � ������ ������ ����� �������� � ������� �� ������ ����� ������ ������ ���� �� 1 ������ �����.
             * �� ���� �������� ���������� ����� ������� ����������, ������ ���������� �� ��������� ������� �����, ����� ���������� ������ ���� ������ ���� */
            MaxBiasCoord.Y = (Bias.Y + (OriginalTileSize.Y - DistanceToAppearanceOfFirstNewTile.Y)) / OriginalTileSize.Y;
        }
        else
            /* ��� ������, ���������� � ������� ��������, ��������� �������� ��������� ����� - ��� ������ ����� ����� ���� ����� � ������
             * ���������� ������, ����� �������� � ���������� �� ������� ������� ���������� ������ ������ �� ��������� ����� ������� */
            MinBiasCoord.Y = MaxBiasCoord.Y = SizeDifference.Y / 2.0 / OriginalTileSize.Y;
    }
    //����� ����� ������� ������� ����
    else {
        //���� ������ �� ��������� � ���� ��������, �� ��� ��������� ���������� � ������� ������
        if (Bias.Y > -(SizeDifference.Y / 2.0)) {
            /* �������� ������������, � ���������� �������, ���������� ��� ������ ������ ������ ���� ������ �������� �� �������� ������ ������� �����.
             * ���������� �� ��������� ������� ������ ����� ����� ���������� �� ������ ��������, ������� ����� ������ ������� ��� �������������, �����
             * ������, ��� ������ ���� ����� ������� ������ ���������, � ������ � ������ ������ ������ ������� �������� � ���������� �� ��������� ������
             * ����� ������� ������ ���� �� 1 ������ �����. �� ���� �������� ���������� ����� ������� ����������, ������ ������ ������� ����� �������
             * ����� � ����������� �� ��������� ������� �����, ����� ������� ������ ���� ������ ���� */
            MaxBiasCoord.Y = (Bias.Y - DistanceToAppearanceOfFirstNewTile.Y) / OriginalTileSize.Y;
            /* �������� �����������, � ���������� ������, ���������� ��� ������ ������ ������ ���� ������ �������� �� ����������� ����� ������ �����.
             * ������� ������ ����� � ���������� �� ��������� ������� ������ ����� ����� ���������� �� ������ ��������, ������� ����� ������ �������
             * ��� �������������, ����� ������, ��� ������ ���� ����� ���������� ������ ���������, � ������ � ������ ������ ������ ������� �������� �
             * ������� �� ������ ����� ������� ������ ���� �� 1 ������ �����. �� ���� �������� ���������� ����� ������� ����������, ������ ������
             * ���������� �� ��������� ������� �����, ����� ���������� ����� ���� ������ ���� */
            MinBiasCoord.Y = (Bias.Y - (OriginalTileSize.Y - DistanceToAppearanceOfFirstNewTile.Y)) / OriginalTileSize.Y;
        }
        else
            /* ��� ������, ���������� � ������� ��������, ��������� �������� ��������� ����� - ��� ������ ����� ����� ���� ����� � ������
             * ���������� ������, ����� �������� � ������ ���������� �� ������� ������� ���������� ������ ����� �� ��������� ����� ������� */
            MinBiasCoord.Y = MaxBiasCoord.Y = -SizeDifference.Y / 2.0 / OriginalTileSize.Y;
    }

    //���������� ������, ���������� ����� ��������, ������������ � �������� ������� ����� ����������� ���������� �������� � ������� ����������
    FDimensionsDisplayedArea BiansDimentions = FDimensionsDisplayedArea(MinBiasCoord, MaxBiasCoord);

    CurrentDimensions = OriginalDimensions + BiansDimentions;

    //��������, ������������ ������� ����� ����������� ��������� � �������
    FDimensionsDisplayedArea ZoomDimentions = FDimensionsDisplayedArea();

    //��������� �������� ������ 1 ��������� ����������� � ������ �������� �� ���������
    if (ZoomMultiplier != 1)
    {
        //����������� ������, ���������� ���������� ������ �������� �������� ��-�� ��� ���������������, ������� ����� ���������� � ����������� ����������� ��� ���������� �������� ������ ��������
        FTileCoord MinZoomCoord;
        FTileCoord MaxZoomCoord;

        FVector2D CurrentTileSize = OriginalTileSize * ZoomMultiplier;
        /* ������� � �������� ����� ����������� �������� �������� � �������. �������� �� 2 ������, ��� ������
           ��������� ������ ������� �� ���� �������, �������� �� ������, � �� ����� ������� �� ���������� x. */
        FVector2D SizeDifferenceBetweenOriginalAndCurrentContentSize = (OriginalDimensionsSize - OriginalDimensionsSize * ZoomMultiplier) / 2.0;

        /* ������ ������������ ������ ������� ������� � �������� ����� ����������� �������� �������� � �������, �� ���� ������ SizeDifferenceBetweenOriginalAndCurrentContentSize.
         * �� ������ ��� ������ ��� ������ ������� ������ ���� �������. ��-������ ���������� �������� �������� � ��� ������� ������������� �� �����. ����������� ������ �������
         * ����� ������� �� ��������� ����� �������, � ��� ��� �������� ����� � ��� �� �����, �� � �������� � ������ ���. ����� ������ �� ������� � ������ ������ ������� ������
         * ��-�� �������� ����� ������ ������� �������� ����������� ������ DistanceToAppearanceOfFirstNewTile �� ��������� ����. ��� ����� �� ����� �����, �� ����������� ���������
         * � ������ � ������� �������, ��� ��� �� ������ �������. ���������� ���������� ���������� �� ����������� ������� � ��������. ������ �������� ����������� � ���, ��� ������
         * �� ������ ��������������, �� � ����������. ���� ����� ������ �������� ����� �������� ����������� �� ��� �� �� ������, ��� � ���� ����������, �� �������� � ���������� ��
         * ����������� ������ �����, ��� ���� � ������ ���� � �������, � � ������ - � ������� �������. ����� ������ ���� ����� (Bias) ���� ��� ���������. �� ����� ��������� �� �����
         * �����, � ����� �� ��������� ������ ���������� ����� �� ���� ������, �� ���� ����� �����, ������� ������� �� ���� ���������� ��������� ����� ������ �� ������. ���� �������,
         * �������� ��� �� ������� ��� �������� ����� ������, �� ������� ������� �������� ����� �������� �� ������ � ���������� ��������� � ������� �������. ����� ���������� ����������
         * ������ ������� �� ������� ������ ����� � ����������� �����, ���� ���� ���� ���������� ���� ���� �� �� ��������� ��������� ��-�� ������ �������. */
        MaxZoomCoord.X = ceil((SizeDifferenceBetweenOriginalAndCurrentContentSize.X - DistanceToAppearanceOfFirstNewTile.X * ZoomMultiplier +
            (Bias.X - MaxBiasCoord.X * OriginalTileSize.X)) / CurrentTileSize.X);

        //���� �� �����-���� ������� ������������ ��������� �������� �� ���� ����� ������ 0, �� ��� ������������. ��� ������� �� ��������� ������� ����� ����������� ������� �������
        if (MaxZoomCoord.X < 0)
            MaxZoomCoord.X = 0;

        /* ���������� ������������ ������ �� ���� � ����������� ���������� ����� ������ �� ��, ��� ��� ���������� � ������������. ���� ���� ���� ������� - ������ ����������� ������,
         * �� ����������, � �������� ����� ���������� �������� �������������. ��� ���������� ��� ��� ��� ���������� ����� ���������� � ���������, � ����������� ���������� ������
         * �������� �������� ����� �������� �����. � ��� ������������� ����� ������ ���������� MinZoomCoord ������ ���� ������������� */
        MinZoomCoord.X = -ceil((SizeDifferenceBetweenOriginalAndCurrentContentSize.X - DistanceToAppearanceOfFirstNewTile.X * ZoomMultiplier -
            (Bias.X - MinBiasCoord.X * OriginalTileSize.X)) / CurrentTileSize.X);

        //���� �� �����-���� ������� ����������� ��������� �������� �� ���� ����� ������ 0, �� ��� ������������. ��� ������� �� ��������� ������� ����� ����������� ������� �������
        if (MinZoomCoord.X > 0)
            MinZoomCoord.X = 0;

        /* ������������ �������� �� ��������� �� ���������� ������������ ������ �������� ���������� ������. ��, ����� ������ ����� ���������
         * � ������� ��������� �������� ������ �� ���� ������������� ������� ��-�� �������� ���������� float'�� � ���������, �� �������� �������
         * �������� ����� ����� ��������, �������� ��-�� �������� ����� �������. ���� �� ���������� ������ ��� ����������� ������������� �����
         * �� ������� ��������� ���������� ������, ��, ����, ��� ����� �������� ����� ���������� ���� ����� �������� � ���� �������, ������
         * �������� �������� ��� ����� �� ��������� �������� �� ��������������� ������� ��������. ��� ����� ������ ����� ��������� ���� ������
         * ��������� �� �������� ��������� ��������. */
        if (Bias.X > 0 && MaxZoomCoord.X + MaxBiasCoord.X > SizeDifference.X / OriginalTileSize.X / 2.0)
            MaxZoomCoord.X = SizeDifference.X / OriginalTileSize.Y / 2.0 - MaxBiasCoord.X;
        if (Bias.X < 0 && MinZoomCoord.X + MinBiasCoord.X < -SizeDifference.X / OriginalTileSize.X / 2.0)
            MinZoomCoord.X = -(SizeDifference.X / OriginalTileSize.Y / 2.0 + MinBiasCoord.X);

        /* ������� ��������� ����� ������������ ������ �� ���� � ���������� Y ��������� �������� �� ���������� X, ���
         * MaxZoomCoord.Y - ����, ������� ��������� ��� �����, � MinZoomCoord.Y - ���, ������� ��������� ��� ���� */

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

        //���������� ������, ���������� ��������������� ��������, ������������ � �������� ������� ����� ����������� �������� �������� � ��� ������� ��������
        ZoomDimentions = FDimensionsDisplayedArea(MinZoomCoord, MaxZoomCoord);
    }

    CurrentDimensions = CurrentDimensions + ZoomDimentions;

    /* ������ �������, ����� ������ �������� ������ ������������. � ���� ������ ������ ������� ������� �������� ��������
     * ������� ����������� ��������, �� ������ ������ ������������ ��� �� ������� � � ���, ��� ��� �������� �� ���������
     * ���� �� �����, � ��������� ������� ������� �������� �������� ������ ������� ������ �������, ��� ��� �� ���������������
     * ���������� ��� ����� ������ ���� ������ �������� */
    bool CurrentXLessThanMin = OriginalTableSize.X < minContentSize.X;
    bool CurrentYLessThanMin = OriginalTableSize.Y < minContentSize.Y;

    if (TilesCoordWrapper->HasAnyEllements()) {
        //���� ���� �� ���� �������� ������ ������������ �������, �� ������������ ��� ����� �� ��������������� ���
        if (CurrentXLessThanMin) {
            CurrentDimensions.Min.X = 0;
            CurrentDimensions.Max.X = TableTileCols - 1;
        }

        if (CurrentYLessThanMin) {
            CurrentDimensions.Min.Y = 0;
            CurrentDimensions.Max.Y = TableTileRows - 1;
        }

        //��������� ������������ ���� �������� �������� ����������
        if (OldDimensions != CurrentDimensions) {
            //����� ������������� ������� ��� ��������� ������ � ������, ����������� ��� ������ ���������� �������� ��������
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

/* �������, ���������� ������������ � ������ ������ �����, ����� ��� ������� ���������
 * ����������. ������������ ��� ������ ��������� ������� ����� ������������������ */
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

//��������������� ���������� � ����� ��� ������ � ����
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