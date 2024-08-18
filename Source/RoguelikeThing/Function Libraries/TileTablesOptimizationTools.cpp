// Denis Igorevich Frolov did all this. Once there. All things reserved.

#include "RoguelikeThing/Function Libraries/TileTablesOptimizationTools.h"
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(TileTablesOptimizationTools);

UTileTablesOptimizationTools::UTileTablesOptimizationTools()
{
    //��������� GameInstance �� ����
    GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (!GameInstance)
        UE_LOG(TileTablesOptimizationTools, Warning, TEXT("Warning in TileTablesOptimizationTools class in constructor - GameInstance was not retrieved from the world"));
}

//������� ������������ ���������� ����� ������
void UTileTablesOptimizationTools::AsynchronousAreaFilling(FGridDimensions AreaDimensions, int NumberOfMapTilesRows)
{
    FString Dimensions = AreaDimensions.ToString();
    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [AreaDimensions, NumberOfMapTilesRows, Dimensions, this]() {
        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the AsynchronousAreaFilling function: An asynchronous thread for adding new tiles when the dimensions change has been started. Dimensions of new tiles: %s"), *Dimensions);

        if (TilesBuf) {
            for (int col = AreaDimensions.Min.Col; col <= AreaDimensions.Max.Col; col++) {
                for (int row = AreaDimensions.Min.Row; row <= AreaDimensions.Max.Row; row++) {
                    //��������� ��������� �������� �������� ������ � �������� ������
                    AsyncTask(ENamedThreads::GameThread, [AreaDimensions, col, row, NumberOfMapTilesRows, this]() {
                        //����� ���� ������ �� ������ ������
                        UPROPERTY()
                        UAbstractTile* Tile = TilesBuf->GetTile();

                        if (Tile) {
                            //������ ��������������� ������ ��� ���������� ����� ������������� ����� �����, � �� ������ ����, ��� � ����������� ������� UniformGrid
                            Tile->SetMyCoord(FCellCoord((NumberOfMapTilesRows - row) - 1, col));
                            UUniformGridSlot* GridSlot = TilesGridPanel->AddChildToUniformGrid(Tile, (NumberOfMapTilesRows - row) - 1, col);
                            TilesCoordWrapper->AddWidget(row, col, Tile, GridSlot);
                            //��� ��������� ������ ����� �� �����, ���������� ��������������� ����� ��� ������������� ����� �����
                            Tile->OnAddedEvent(MapMatrix);

                            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                                UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the AsynchronousAreaFilling function: Tile Col: %d, Row: %d has been added"), col, row);
                        }
                        else
                            UE_LOG(CoordWrapperOfTable, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the AsynchronousAreaFilling function: Tile from TilesBuf is not valid"));
                        });
                }
            }
        }
        else {
            UE_LOG(CoordWrapperOfTable, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the AsynchronousAreaFilling function: TilesBuf is not valid"));
        }

        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the AsynchronousAreaFilling function: An asynchronous thread for adding new tiles when the dimensions change was terminated. Dimensions of new tiles: %s"), *Dimensions);
    });
}

//������� ������������ �������� ������ ������
void UTileTablesOptimizationTools::AsynchronousAreaRemoving(FGridDimensions AreaDimensions, int NumberOfMapTilesRows)
{
    FString Dimensions = AreaDimensions.ToString();
    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [AreaDimensions, NumberOfMapTilesRows, Dimensions, this]() {
        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the AsynchronousAreaRemoving function: An asynchronous thread for removing old tiles when the dimensions change has been started. Dimensions of removing tiles: %s"), *Dimensions);

        if (TilesBuf) {
            for (int col = AreaDimensions.Min.Col; col <= AreaDimensions.Max.Col; col++) {
                for (int row = AreaDimensions.Min.Row; row <= AreaDimensions.Max.Row; row++) {
                    //��������� ��������� �������� �������� ������ � �������� ������
                    AsyncTask(ENamedThreads::GameThread, [col, row, AreaDimensions, this]() {
                        //���� ����� ������ ��������� �������, ������ ����� ���������
                        if (TilesBuf->BufSize() + 1 > TilesBuf->GetMaxSize()) {
                            if (!TilesCoordWrapper->RemoveWidget(row, col)) {
                                UE_LOG(CoordWrapperOfTable, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the AsynchronousAreaRemoving function: Failed to remove tile Col: %d, Row: %d from TilesCoordWrapper"), col, row);
                            }

                            //��� ��� ��������� �������� �������, ������� ����������� ������ ������
                            GetWorld()->ForceGarbageCollection(true);
                        }
                        //����� �� ������������ � ����� ������
                        else {
                            UUniformGridSlot* GridSlot = TilesCoordWrapper->FindGridSlot(row, col);
                            if (GridSlot) {
                                //��� ���� �� ������� �� ���������, ������ ������������ �� ������� ��������� � ������ � ������������� ���������
                                GridSlot->SetColumn(-1);
                                GridSlot->SetRow(-1);

                                UAbstractTile* Tile = TilesCoordWrapper->FindWidget(row, col);
                                if (Tile) {
                                    //��������� ����������� ����� ����� ������������
                                    Tile->ClearFilledCells();

                                    //�� ������������ ������ ��������� ���������� �����
                                    if (!TilesCoordWrapper->RemoveCoord(row, col)) {
                                        UE_LOG(CoordWrapperOfTable, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the AsynchronousAreaRemoving function: Failed to remove tile coordinate Col: %d, Row: %d from TilesCoordWrapper"), col, row);
                                    }

                                    //� ����� ���� ���� ����������� � �����
                                    TilesBuf->AddTile(Tile);
                                }
                                else {
                                    UE_LOG(CoordWrapperOfTable, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the AsynchronousAreaRemoving function: Tile is not valid"));
                                }
                            }
                            else {
                                UE_LOG(CoordWrapperOfTable, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the AsynchronousAreaRemoving function: GridSlot of tile is not valid"));
                            }
                        }
                        });
                }
            }
        }
        else {
            UE_LOG(CoordWrapperOfTable, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the AsynchronousAreaRemoving function: TilesBuf is not valid"));
        }

        if(GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the AsynchronousAreaRemoving function: An asynchronous thread for removing old tiles when the dimensions change was terminated. Dimensions of removing tiles: %s"), *Dimensions);
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
    OldDimensions = OriginalDimensions;
    CurrentDimensions = OriginalDimensions;

    FGridCoord MaxCoord = OriginalDimensions.Max;
    FGridCoord MinCoord = OriginalDimensions.Min;
    OriginalDimensionsSize = FVector2D((MaxCoord.Col - MinCoord.Col) * TileSize.X, (MaxCoord.Row - MinCoord.Row) * TileSize.Y);

    MaximumContentSize = FVector2D(NumberOfTileColsInTable * TileSize.X, NumberOfTileRowsInTable * TileSize.Y);
    SizeDifference = MaximumContentSize - WidgetAreaSize;

    //���� ������ �������� ������, ��� �����������, �� ������� � ������� ���������� � ���������������� ��� ������� �� ����� - ��� � ��� ��� ����� �������������
    if (MaximumContentSize.X < MinContentSize.X)
        SizeDifference.X = 0;
    if (MaximumContentSize.Y < MinContentSize.Y)
        SizeDifference.Y = 0;

    /* ����������, ��� ����������� �������� ���������� ������ ����� ���� ����������� ���� ���������� �������� �������
     * ����� ��������� ���������� ������� ������� � ������� ������������� ��������. ��� ����� ��������� ������, ������
     * ������ �������, ������� ��� �� ������ �� ��������� �������, ������ �������� �������� �������� ������� */
    DistanceToAppearanceOfFirstNewTile = (OriginalDimensionsSize - WidgetAreaSize) / 2.0;

    IsInit = true;
}

//�������, ���������� ���������� ������������ ������ �� ������ ��� ��������������� �������
void UTileTablesOptimizationTools::ChangingVisibilityOfTableTiles(FVector2D Bias, float ZoomMultiplier, int NumberOfMapTilesRows)
{
    if (!IsInit) {
        UE_LOG(CoordWrapperOfTable, Error, TEXT("!!! An error occurred in the TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The instance is not initialized"));
        return;
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

    //ZoomMultiplier �� ����� ���� ����� 0, ���� ��� ���, �� �� ������������ �� �������
    if (ZoomMultiplier == 0)
        ZoomMultiplier = 1;

    //������������� ������� ������� � �������� � ������ ����
    FVector2D CurrentSizeDifference = MaximumContentSize - (WidgetAreaSize / ZoomMultiplier);

    //����������� ������, ���������� ����� ��������, ������� ����� ���������� � ����������� ����������� ��� ���������� �������� ������� ��������
    FGridCoord MinBiasCoord;
    FGridCoord MaxBiasCoord;

    //�������� �� X ������ 0 �������� ����� ������� ������� ������
    if (Bias.X > 0) {
        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The visible area has moved to the right of the center"));

        //���� ������ �� ��������� � ���� ��������, �� ��� ��������� ���������� � ������� ������
        if (Bias.X < CurrentSizeDifference.X / 2.0 - TileSize.X) {
            /* �������� �����������, � ���������� �����, ���������� ��� ������ ������ ����� ������� ������ �������� �� ��������� ������ ����� �����.
             * ���������� �� ��������� ������� ������ ����� ����� ������������ � ������ ��������, ����� ������, ��� ������ ���� ����� �������� ������
             * ���������, � ������ � ������ ������ ����� �������� � ���������� �� ��������� ������ ����� ������ ������ ���� �� 1 ������ �����. �� ����
             * �������� ���������� ����� ������� ����������, ������ ������� ����� ������� ����� � ����������� �� ��������� ������� �����, ����� �������
             * ����� ���� ������ ���� */
            MinBiasCoord.Col = (Bias.X + DistanceToAppearanceOfFirstNewTile.X) / TileSize.X;
            /* �������� ������������, � ���������� ������, ���������� ��� ������ ������ ����� ������� ������ �������� �� ����������� ����� ������ �����.
             * ������� ������ ����� � ���������� �� ��������� ������� ������ ����� ����� ������������ � ������ ��������, ����� ������, ��� ������ ����
             * ����� ���������� ������ ���������, � ������ � ������ ������ ����� �������� � ������� �� ������ ����� ������ ������ ���� �� 1 ������ �����.
             * �� ���� �������� ���������� ����� ������� ����������, ������ ���������� �� ��������� ������� �����, ����� ���������� ������ ���� ������ ���� */
            MaxBiasCoord.Col = (Bias.X + (TileSize.X - DistanceToAppearanceOfFirstNewTile.X)) / TileSize.X;
        }
        else {
            /* ��� ������, ���������� � ������� ��������, ��������� �������� ��������� ����� - ��� ������ ����� ����� ���� ����� � ������
             * ���������� ������, ����� �������� � ���������� �� ������� ������� ���������� ������ ������ �� ��������� ����� ������� */
            MinBiasCoord.Col = MaxBiasCoord.Col = SizeDifference.X / 2.0 / TileSize.X;

            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The content rests against the right edge of the widget"));
        }
    }
    //����� ����� ������� ������� �����
    else {
        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The visible area has moved to the left of the center"));

        //���� ������ �� ��������� � ���� ��������, �� ��� ��������� ���������� � ������� ������
        if (Bias.X > -(CurrentSizeDifference.X / 2.0 - TileSize.X)) {
            /* �������� ������������, � ���������� ������, ���������� ��� ������ ������ ������� ������ ������ �������� �� �������� ������ ������ �����.
             * ���������� �� ��������� ������� ������ ����� ����� ���������� �� ������ ��������, ������� ����� ������ ������� ��� �������������, �����
             * ������, ��� ������ ���� ����� ������� ������ ���������, � ������ � ������ ������ ������ ������� �������� � ���������� �� ��������� ������
             * ����� ������� ������ ���� �� 1 ������ �����. �� ���� �������� ���������� ����� ������� ����������, ������ ������ ������� ����� �������
             * ����� � ����������� �� ��������� ������� �����, ����� ������� ������ ���� ������ ���� */
            MaxBiasCoord.Col = (Bias.X - DistanceToAppearanceOfFirstNewTile.X) / TileSize.X;
            /* �������� �����������, � ���������� �����, ���������� ��� ������ ������ ������� ������ ������ �������� �� ����������� ����� ����� �����.
             * ������� ������ ����� � ���������� �� ��������� ������� ������ ����� ����� ���������� �� ������ ��������, ������� ����� ������ �������
             * ��� �������������, ����� ������, ��� ������ ���� ����� ���������� ������ ���������, � ������ � ������ ������ ������ ������� �������� �
             * ������� �� ������ ����� ������� ������ ���� �� 1 ������ �����. �� ���� �������� ���������� ����� ������� ����������, ������ ������
             * ���������� �� ��������� ������� �����, ����� ���������� ����� ���� ������ ���� */
            MinBiasCoord.Col = (Bias.X - (TileSize.X - DistanceToAppearanceOfFirstNewTile.X)) / TileSize.X;
        }
        else {
            /* ��� ������, ���������� � ������� ��������, ��������� �������� ��������� ����� - ��� ������ ����� ����� ���� ����� � ������
             * ���������� ������, ����� �������� � ������ ���������� �� ������� ������� ���������� ������ ����� �� ��������� ����� ������� */
            MinBiasCoord.Col = MaxBiasCoord.Col = -SizeDifference.X / 2.0 / TileSize.X;

            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The content rests against the left edge of the widget"));
        }
    }

    //�������� �� Y ������ 0 �������� ����� ������� ������� �����
    if (Bias.Y > 0) {
        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The visible area has moved to the top of the center"));

        //���� ������ �� ��������� � ���� ��������, �� ��� ��������� ���������� � ������� ������
        if (Bias.Y < CurrentSizeDifference.Y / 2.0 - TileSize.Y) {
            /* �������� �����������, � ���������� ������, ���������� ��� ������ ������ ����� ����� ������ �������� �� ��������� ������ ������ �����.
             * ���������� �� ��������� ������� ������ ����� ����� ������������ � ������ ��������, ����� ������, ��� ������ ���� ����� �������� ������
             * ���������, � ������ � ������ ������ ����� �������� � ���������� �� ��������� ������ ����� ������ ������ ���� �� 1 ������ �����. �� ����
             * �������� ���������� ����� ������� ����������, ������ ������� ����� ������� ����� � ����������� �� ��������� ������� �����, ����� �������
             * ����� ���� ������ ���� */
            MinBiasCoord.Row = (Bias.Y + DistanceToAppearanceOfFirstNewTile.Y) / TileSize.Y;
            /* �������� ������������, � ���������� �������, ���������� ��� ������ ������ ����� ����� ������ �������� �� ����������� ����� ������� �����.
             * ������� ������ ����� � ���������� �� ��������� ������� ������ ����� ����� ������������ � ������ ��������, ����� ������, ��� ������ ����
             * ����� ���������� ������ ���������, � ������ � ������ ������ ����� �������� � ������� �� ������ ����� ������ ������ ���� �� 1 ������ �����.
             * �� ���� �������� ���������� ����� ������� ����������, ������ ���������� �� ��������� ������� �����, ����� ���������� ������ ���� ������ ���� */
            MaxBiasCoord.Row = (Bias.Y + (TileSize.Y - DistanceToAppearanceOfFirstNewTile.Y)) / TileSize.Y;
        }
        else {
            /* ��� ������, ���������� � ������� ��������, ��������� �������� ��������� ����� - ��� ������ ����� ����� ���� ����� � ������
             * ���������� ������, ����� �������� � ���������� �� ������� ������� ���������� ������ ������ �� ��������� ����� ������� */
            MinBiasCoord.Row = MaxBiasCoord.Row = SizeDifference.Y / 2.0 / TileSize.Y;

            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The content rests against the top edge of the widget"));
        }
    }
    //����� ����� ������� ������� ����
    else {
        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The visible area has moved to the bottom of the center"));

        //���� ������ �� ��������� � ���� ��������, �� ��� ��������� ���������� � ������� ������
        if (Bias.Y > -(CurrentSizeDifference.Y / 2.0 - TileSize.Y)) {
            /* �������� ������������, � ���������� �������, ���������� ��� ������ ������ ������ ���� ������ �������� �� �������� ������ ������� �����.
             * ���������� �� ��������� ������� ������ ����� ����� ���������� �� ������ ��������, ������� ����� ������ ������� ��� �������������, �����
             * ������, ��� ������ ���� ����� ������� ������ ���������, � ������ � ������ ������ ������ ������� �������� � ���������� �� ��������� ������
             * ����� ������� ������ ���� �� 1 ������ �����. �� ���� �������� ���������� ����� ������� ����������, ������ ������ ������� ����� �������
             * ����� � ����������� �� ��������� ������� �����, ����� ������� ������ ���� ������ ���� */
            MaxBiasCoord.Row = (Bias.Y - DistanceToAppearanceOfFirstNewTile.Y) / TileSize.Y;
            /* �������� �����������, � ���������� ������, ���������� ��� ������ ������ ������ ���� ������ �������� �� ����������� ����� ������ �����.
             * ������� ������ ����� � ���������� �� ��������� ������� ������ ����� ����� ���������� �� ������ ��������, ������� ����� ������ �������
             * ��� �������������, ����� ������, ��� ������ ���� ����� ���������� ������ ���������, � ������ � ������ ������ ������ ������� �������� �
             * ������� �� ������ ����� ������� ������ ���� �� 1 ������ �����. �� ���� �������� ���������� ����� ������� ����������, ������ ������
             * ���������� �� ��������� ������� �����, ����� ���������� ����� ���� ������ ���� */
            MinBiasCoord.Row = (Bias.Y - (TileSize.Y - DistanceToAppearanceOfFirstNewTile.Y)) / TileSize.Y;
        }
        else {
            /* ��� ������, ���������� � ������� ��������, ��������� �������� ��������� ����� - ��� ������ ����� ����� ���� ����� � ������
             * ���������� ������, ����� �������� � ������ ���������� �� ������� ������� ���������� ������ ����� �� ��������� ����� ������� */
            MinBiasCoord.Row = MaxBiasCoord.Row = -SizeDifference.Y / 2.0 / TileSize.Y;

            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The content rests against the bottom edge of the widget"));
        }
    }

    //���������� ������, ���������� ����� ��������, ������������ � �������� ������� ����� ����������� ���������� �������� � ������� ����������
    FGridDimensions BiasDimentions = FGridDimensions(MinBiasCoord, MaxBiasCoord);

    if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
        UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: With � visible area bias %s, the current bias dimensions are equal to %s"), *Bias.ToString(), *BiasDimentions.ToString());

    CurrentDimensions = OriginalDimensions + BiasDimentions;

    //��������, ������������ ������� ����� ����������� ��������� � �������
    FGridDimensions ZoomDimentions = FGridDimensions(FGridCoord(0, 0), FGridCoord(0, 0));

    //��������� �������� ������ 1 ��������� ����������� � ������ �������� �� ���������
    if (ZoomMultiplier != 1)
    {
        //����������� ������, ���������� ���������� ������ �������� �������� ��-�� ��� ���������������, ������� ����� ���������� � ����������� ����������� ��� ���������� �������� ������ ��������
        FGridCoord MinZoomCoord;
        FGridCoord MaxZoomCoord;

        FVector2D CurrentTileSize = TileSize * ZoomMultiplier;

        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: Current tile size is %s"), *CurrentTileSize.ToString());

        /* ������� � �������� ����� ����������� �������� �������� � �������. �������� �� 2 ������, ��� ������
           ��������� ������ ������� �� ���� �������, �������� �� ������, � �� ����� ������� �� ���������� x. */
        FVector2D SizeDifferenceBetweenOriginalAndCurrentContentSize = (OriginalDimensionsSize - OriginalDimensionsSize * ZoomMultiplier) / 2.0;

        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: Size difference between original and current content size on each side is %s"), *SizeDifferenceBetweenOriginalAndCurrentContentSize.ToString());

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
        MaxZoomCoord.Col = ceil((SizeDifferenceBetweenOriginalAndCurrentContentSize.X - DistanceToAppearanceOfFirstNewTile.X * ZoomMultiplier +
            (Bias.X - MaxBiasCoord.Col * TileSize.X)) / CurrentTileSize.X);

        //���� �� �����-���� ������� ������������ ��������� �������� �� ���� ����� ������ 0, �� ��� ������������. ��� ������� �� ��������� ������� ����� ����������� ������� �������
        if (MaxZoomCoord.Col < 0)
            MaxZoomCoord.Col = 0;

        /* ���������� ������������ ������ �� ���� � ����������� ���������� ����� ������ �� ��, ��� ��� ���������� � ������������. ���� ���� ���� ������� - ������ ����������� ������,
         * �� ����������, � �������� ����� ���������� �������� �������������. ��� ���������� ��� ��� ��� ���������� ����� ���������� � ���������, � ����������� ���������� ������
         * �������� �������� ����� �������� �����. � ��� ������������� ����� ������ ���������� MinZoomCoord ������ ���� ������������� */
        MinZoomCoord.Col = -ceil((SizeDifferenceBetweenOriginalAndCurrentContentSize.X - DistanceToAppearanceOfFirstNewTile.X * ZoomMultiplier -
            (Bias.X - MinBiasCoord.Col * TileSize.X)) / CurrentTileSize.X);

        //���� �� �����-���� ������� ����������� ��������� �������� �� ���� ����� ������ 0, �� ��� ������������. ��� ������� �� ��������� ������� ����� ����������� ������� �������
        if (MinZoomCoord.Col > 0)
            MinZoomCoord.Col = 0;
        /* ������������ �������� �� ��������� �� ���������� ������������ ������ �������� ���������� ������. ��, ����� ������ ����� ���������
         * � ������� ��������� �������� ������ �� ���� ������������� ������� ��-�� �������� ���������� float'�� � ���������, �� �������� �������
         * �������� ����� ����� ��������, �������� ��-�� �������� ����� �������. ���� �� ���������� ������ ��� ����������� ������������� �����
         * �� ������� ��������� ���������� ������, ��, ����, ��� ����� �������� ����� ���������� ���� ����� �������� � ���� �������, ������
         * �������� �������� ��� ����� �� ��������� �������� �� ��������������� ������� ��������. ��� ����� ������ ����� ��������� ���� ������
         * ��������� �� �������� ��������� ��������. */
        if (Bias.X > 0 && MaxZoomCoord.Col + MaxBiasCoord.Col > SizeDifference.X / TileSize.X / 2.0)
            MaxZoomCoord.Col = SizeDifference.X / TileSize.Y / 2.0 - MaxBiasCoord.Col;
        if (Bias.X < 0 && MinZoomCoord.Col + MinBiasCoord.Col < -SizeDifference.X / TileSize.X / 2.0)
            MinZoomCoord.Col = -(SizeDifference.X / TileSize.Y / 2.0 + MinBiasCoord.Col);
        /* ������� ��������� ����� ������������ ������ �� ���� � ���������� Y ��������� �������� �� ���������� X, ���
         * MaxZoomCoord.Y - ����, ������� ��������� ��� �����, � MinZoomCoord.Y - ���, ������� ��������� ��� ���� */
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

        //���������� ������, ���������� ��������������� ��������, ������������ � �������� ������� ����� ����������� �������� �������� � ��� ������� ��������
        ZoomDimentions = FGridDimensions(MinZoomCoord, MaxZoomCoord);

        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: With a zoom multiplier of %f, the current zoom dimensions are equal to %s"), ZoomMultiplier, *ZoomDimentions.ToString());
    }
    else if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
        UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: ZoomMultiplier is equal to 1 - no scaling"));

    CurrentDimensions = CurrentDimensions + ZoomDimentions;

    /* ������ �������, ����� ������ �������� ������ ������������. � ���� ������ ������ ������� ������� �������� ��������
     * ������� ����������� ��������, �� ������ ������ ������������ ��� �� ������� � � ���, ��� ��� �������� �� ���������
     * ���� �� �����, � ��������� ������� ������� �������� ������ ������ ������� ������ �������, ��� ��� �� ���������������
     * ���������� ��� ����� ������ ���� ������ �������� */
    bool CurrentXLessThanMin = MaximumContentSize.X < MinContentSize.X;
    bool CurrentYLessThanMin = MaximumContentSize.Y < MinContentSize.Y;

    if (TilesCoordWrapper->HasAnyEllements()) {
        //���� ���� �� ���� �������� ������ ������������ �������, �� ������������ ��� ����� �� ��������������� ���
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

        //��������� ������������ ���� �������� �������� ����������
        if (OldDimensions != CurrentDimensions) {
            //UE_LOG(TileTablesOptimizationTools, Log, TEXT("old: %s, new %s"), *OldDimensions.ToString(), *CurrentDimensions.ToString());

            //���� ������ � ������� ������� ���������� ���� �����, �� ����������� ��� �� ������������ ���������
            if (OldDimensions.DoTheDimensionsIntersect(CurrentDimensions)) {
                //����� ������������� ������� ��� ��������� ������ � ������, ����������� ��� ������ ���������� �������� ��������
                bool NewTopBoundMore = (CurrentDimensions.Max.Row > OldDimensions.Max.Row);
                bool OldTopBoundLess = CurrentDimensions.Max.Row < OldDimensions.Max.Row;
                bool NewBottomBoundMore = CurrentDimensions.Min.Row < OldDimensions.Min.Row;
                bool OldBottomBoundLess = CurrentDimensions.Min.Row > OldDimensions.Min.Row;
                bool NewLeftBoundMore = CurrentDimensions.Min.Col < OldDimensions.Min.Col;
                bool OldLeftBoundLess = CurrentDimensions.Min.Col > OldDimensions.Min.Col;
                bool NewRightBoundMore = CurrentDimensions.Max.Col > OldDimensions.Max.Col;
                bool OldRightBoundLess = CurrentDimensions.Max.Col < OldDimensions.Max.Col;

                //���� ������ �������� �� Y ������ ������������, �� ��� ��������, ��� ��� ����� �� ��������� � ��� ������ ����������, � ��� ������ ������ ��� �������� � �����
                if (!CurrentYLessThanMin) {
                    //���� ���� ����������������, ��� ����� ������� ������� ���� ������� ������� ���������� �������, �� ��� ����������� ����� ������ ���������� ��������
                    if (NewTopBoundMore) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is higher than the top of the previous area"));
                        if (!NewLeftBoundMore && !NewRightBoundMore) {
                            /* ��� ��� ��������, ��� ����� ������� ���� ����������, ���� �������� �� ������ �� 1 ����, ��� ���������� �������
                             * �������, ����� ��������� ������ ������������� �����, � �������� �� ����� ������� ������� ������������ */
                            AsynchronousAreaFilling(FGridDimensions(FGridCoord(OldDimensions.Max.Row + 1, CurrentDimensions.Min.Col),
                                FGridCoord(CurrentDimensions.Max.Row, CurrentDimensions.Max.Col)), NumberOfMapTilesRows);
                        }
                        else {
                            if (NewLeftBoundMore) {
                                AsynchronousAreaFilling(FGridDimensions(FGridCoord(OldDimensions.Max.Row + 1, OldDimensions.Min.Col),
                                    FGridCoord(CurrentDimensions.Max.Row, CurrentDimensions.Max.Col)), NumberOfMapTilesRows);
                            }

                            if (NewRightBoundMore) {
                                AsynchronousAreaFilling(FGridDimensions(FGridCoord(OldDimensions.Max.Row + 1, CurrentDimensions.Min.Col),
                                    FGridCoord(CurrentDimensions.Max.Row, OldDimensions.Max.Col)), NumberOfMapTilesRows);
                            }
                        }
                    }

                    //���� ���� ����������������, ��� ����� ������� ������� ���� ������� ������� ���������� �������, �� ��� ����������� ������ ������ ���������� ������������������
                    if (OldTopBoundLess) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is below the top of the previous area"));

                        if (!OldLeftBoundLess && !OldRightBoundLess) {
                            /* ��� ��� ��������, ��� ����� ������� ���� ����������, ���� �������� �� ������ �� 1 ����, ��� ������� �������
                             * �������, ����� ��������� ������ ����������� �����, � �������� �� ������ ������� ������� ������������ */
                            AsynchronousAreaRemoving(FGridDimensions(FGridCoord(CurrentDimensions.Max.Row + 1, OldDimensions.Min.Col),
                                FGridCoord(OldDimensions.Max.Row, OldDimensions.Max.Col)), NumberOfMapTilesRows);
                        }
                        else {
                            if (OldLeftBoundLess) {
                                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(CurrentDimensions.Max.Row + 1, CurrentDimensions.Min.Col),
                                    FGridCoord(OldDimensions.Max.Row, OldDimensions.Max.Col)), NumberOfMapTilesRows);
                            }

                            if (OldRightBoundLess) {
                                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(CurrentDimensions.Max.Row + 1, OldDimensions.Min.Col),
                                    FGridCoord(OldDimensions.Max.Row, CurrentDimensions.Max.Col)), NumberOfMapTilesRows);
                            }
                        }
                    }

                    //���� ���� ����������������, ��� ����� ������� ������� ���� ������ ������� ���������� �������, �� ��� ����������� ����� ������ ���������� ��������
                    if (NewBottomBoundMore) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is below the lower boundary of the previous area"));

                        if (!NewLeftBoundMore && !NewRightBoundMore) {
                            /* ��� ��� ��������, ��� ����� ������� ���� ����������, ���� �������� �� ������ ������ ������� ������ �������, � ��������
                             * �� ������, ������������� �� 1 ����, ��� ������ ������ �������, ����� ��������� ������ ������������� ����� */
                            AsynchronousAreaFilling(FGridDimensions(FGridCoord(CurrentDimensions.Min.Row, CurrentDimensions.Min.Col),
                                FGridCoord(OldDimensions.Min.Row - 1, CurrentDimensions.Max.Col)), NumberOfMapTilesRows);
                        }
                        else {
                            if (NewLeftBoundMore) {
                                AsynchronousAreaFilling(FGridDimensions(FGridCoord(CurrentDimensions.Min.Row, OldDimensions.Min.Col),
                                    FGridCoord(OldDimensions.Min.Row - 1, CurrentDimensions.Max.Col)), NumberOfMapTilesRows);
                            }

                            if (NewRightBoundMore) {
                                AsynchronousAreaFilling(FGridDimensions(FGridCoord(CurrentDimensions.Min.Row, CurrentDimensions.Min.Col),
                                    FGridCoord(OldDimensions.Min.Row - 1, OldDimensions.Max.Col)), NumberOfMapTilesRows);
                            }
                        }
                    }

                    //���� ���� ����������������, ��� ����� ������� ������� ���� ������ ������� ���������� �������, �� ��� ����������� ������ ������ ���������� ������������������
                    if (OldBottomBoundLess) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is higher than the lower border of the previous area"));

                        if (!OldLeftBoundLess && !OldRightBoundLess) {
                            /* ��� ��� ��������, ��� ����� ������� ���� ����������, ���� �������� �� ������ ������ ������ ������ �������, �
                             * �������� �� ������, ������������� �� 1 ����, ��� ������� ������ �������, ����� ��������� ������ ����������� ����� */
                            AsynchronousAreaRemoving(FGridDimensions(FGridCoord(OldDimensions.Min.Row, OldDimensions.Min.Col),
                                FGridCoord(CurrentDimensions.Min.Row - 1, OldDimensions.Max.Col)), NumberOfMapTilesRows);
                        }
                        else {
                            if (OldLeftBoundLess) {
                                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(OldDimensions.Min.Row, CurrentDimensions.Min.Col),
                                    FGridCoord(CurrentDimensions.Min.Row - 1, OldDimensions.Max.Col)), NumberOfMapTilesRows);
                            }
                            if (OldRightBoundLess) {
                                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(OldDimensions.Min.Row, OldDimensions.Min.Col),
                                    FGridCoord(CurrentDimensions.Min.Row - 1, CurrentDimensions.Max.Col)), NumberOfMapTilesRows);
                            }
                        }
                    }
                }

                //���� ������ �������� �� X ������ ������������, �� ��� ��������, ��� ��� ����� �� ����������� � ��� ������ ����������, � ��� ������ ������ ��� �������� � �����
                if (!CurrentXLessThanMin) {
                    //���� ���� ����������������, ��� ����� ������� ������� ����� ����� ������� ���������� �������, �� ��� ����������� ����� ������� ���������� ��������
                    if (NewLeftBoundMore) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The current area is to the left of the left border of the previous area"));

                        AsynchronousAreaFilling(FGridDimensions(FGridCoord(CurrentDimensions.Min.Row, CurrentDimensions.Min.Col),
                            FGridCoord(CurrentDimensions.Max.Row, OldDimensions.Min.Col - 1)), NumberOfMapTilesRows);
                    }

                    //���� ���� ����������������, ��� ����� ������� ������� ������ ����� ������� ���������� �������, �� ��� ����������� ������ ������� ���������� ������������������
                    if (OldLeftBoundLess) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is to the right of the left border of the previous area"));

                        /* ��� ��� ��������, ��� ����� ������� ������ ����������, ���� �������� �� ������� ������� ������ ����� �������, �
                         * �������� �� �������, �������������� �� 1 �����, ��� ������� ����� �������, ����� ��������� ������ ����������� ����� */
                        AsynchronousAreaRemoving(FGridDimensions(FGridCoord(OldDimensions.Min.Row, OldDimensions.Min.Col),
                            FGridCoord(OldDimensions.Max.Row, CurrentDimensions.Min.Col - 1)), NumberOfMapTilesRows);
                    }

                    //���� ���� ����������������, ��� ����� ������� ������� ������ ������ ������� ���������� �������, �� ��� ����������� ����� ������� ���������� ��������
                    if (NewRightBoundMore) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is to the right of the right border of the previous area"));

                        /* ��� ��� ��������, ��� ����� ������� ������ ����������, ���� �������� �� ������� �� 1 ������, ��� ���������� ������
                         * �������, ����� ��������� ������ ������������� �����, � �������� �� ����� ������ ������� ������������ */
                        AsynchronousAreaFilling(FGridDimensions(FGridCoord(CurrentDimensions.Min.Row, OldDimensions.Max.Col + 1),
                            FGridCoord(CurrentDimensions.Max.Row, CurrentDimensions.Max.Col)), NumberOfMapTilesRows);
                    }

                    //���� ���� ����������������, ��� ����� ������� ������� ����� ������ ������� ���������� �������, �� ��� ����������� ������ ������� ���������� ������������������
                    if (OldRightBoundLess) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is to the left of the right border of the previous area"));

                        /* ��� ��� ��������, ��� ����� ������� ����� ����������, ���� �������� �� ������� �� 1 ������, ��� ������� ������
                         * �������, ����� ��������� ������ ����������� �����, � �������� �� ������ ������ ������� ������������ */
                        AsynchronousAreaRemoving(FGridDimensions(FGridCoord(OldDimensions.Min.Row, CurrentDimensions.Max.Col + 1),
                            FGridCoord(OldDimensions.Max.Row, OldDimensions.Max.Col)), NumberOfMapTilesRows);
                    }
                }
            }
            //����� ��� ������ ������� ���������, � ��� ����� ������� �������������� � ����
            else {
                AsynchronousAreaFilling(CurrentDimensions, NumberOfMapTilesRows);
                AsynchronousAreaRemoving(OldDimensions, NumberOfMapTilesRows);
            }

            //������������ ������ �������� � ����� ������������ � ����������� ����������
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