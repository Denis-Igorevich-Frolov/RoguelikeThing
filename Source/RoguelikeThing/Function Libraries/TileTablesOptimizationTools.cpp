// Denis Igorevich Frolov did all this. Once there. All things reserved.

#include "RoguelikeThing/Function Libraries/TileTablesOptimizationTools.h"
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(TileTablesOptimizationTools);

#pragma optimize("", off)

UTileTablesOptimizationTools::UTileTablesOptimizationTools()
{
    //��������� GameInstance �� ����
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
                    else
                        UE_LOG(TileTablesOptimizationTools, Log, TEXT("%s add ddddddd r: %d, c: %d"), *ss, row, col);

                    UUserWidget* Tile = TilesBuf->GetTile();

                    if (Tile) {
                        UUniformGridSlot* GridSlot = TilesGridPanel->AddChildToUniformGrid(Tile, (NumberOfMapTilesRows - row) - 1, col);
                        TilesCoordWrapper->AddWidget(row, col, Tile, GridSlot);
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
    UE_LOG(TileTablesOptimizationTools, Log, TEXT("%s begin remove %s"), *ss, *AreaDimensions.ToString());
    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [ss, AreaDimensions, NumberOfMapTilesRows, this]() {
        GetWorld()->ForceGarbageCollection(true);
        for (int col = AreaDimensions.Min.Col; col <= AreaDimensions.Max.Col; col++) {
                for (int row = AreaDimensions.Min.Row; row <= AreaDimensions.Max.Row; row++) {
                    AsyncTask(ENamedThreads::GameThread, [ss, col, row, AreaDimensions, this]() {
                        if (TilesBuf->BufSize() + 1 > TilesBuf->GetMaxSize()) {
                            if (!TilesCoordWrapper->RemoveWidget(row, col)) {
                                UE_LOG(TileTablesOptimizationTools, Warning, TEXT("%s remove fffffff r: %d, c: %d"), *ss, row, col);
                            }
                            else
                                UE_LOG(TileTablesOptimizationTools, Log, TEXT("%s remove ddddddd r: %d, c: %d"), *ss, row, col);
                        }
                        else {
                            UUniformGridSlot* GridSlot = TilesCoordWrapper->FindGridSlot(row, col);
                            if (GridSlot) {
                                GridSlot->SetColumn(-1);
                                GridSlot->SetRow(-1);
                            }
                            else
                                UE_LOG(TileTablesOptimizationTools, Warning, TEXT("%s remove 2 ssdsdfsdf r: %d, c: %d"), *ss, row, col);

                            UUserWidget* Tile = TilesCoordWrapper->FindWidget(row, col);

                            if(!TilesCoordWrapper->RemoveCoord(row, col)) {
                                UE_LOG(TileTablesOptimizationTools, Warning, TEXT("%s remove 2 fffffff r: %d, c: %d"), *ss, row, col);
                            }
                            else
                                UE_LOG(TileTablesOptimizationTools, Log, TEXT("%s remove 2 ddddddd r: %d, c: %d"), *ss, row, col);

                            TilesBuf->AddTile(Tile);
                        }
                        });
                }
        }
        GetWorld()->ForceGarbageCollection(false);
    });
}

void UTileTablesOptimizationTools::Init(UUniformGridPanel* refTilesGridPanel, UCoordWrapperOfTable* refTilesCoordWrapper, UTileBuffer* refTilesBuf,
    FGridDimensions originalDimensions, FVector2D widgetAreaSize, FVector2D tileSize, FVector2D minContentSize, int numberOfTileRowsInTable, int numberOfTileColsInTable)
{
    TilesGridPanel = refTilesGridPanel;
    TilesCoordWrapper = refTilesCoordWrapper;
    TilesBuf = refTilesBuf;

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

//�������, ���������� ��������� ������ �� ������ ��� ��������������� �������
void UTileTablesOptimizationTools::ChangingVisibilityOfTableTiles(FVector2D Bias, float ZoomMultiplier, int NumberOfMapTilesRows)
{
    if (!IsInit) {
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

    FVector2D CurrentSizeDifference = MaximumContentSize - (WidgetAreaSize / ZoomMultiplier);

    //����������� ������, ���������� ����� ��������, ������� ����� ���������� � ����������� ����������� ��� ���������� �������� ������� ��������
    FGridCoord MinBiasCoord;
    FGridCoord MaxBiasCoord;

    //�������� �� X ������ 0 �������� ����� ������� ������� ������
    if (Bias.X > 0) {
        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The visible area has moved to the right of the center"));

        //���� ������ �� ��������� � ���� ��������, �� ��� ��������� ���������� � ������� ������
        if (Bias.X < CurrentSizeDifference.X / 2.0) {
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
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Right"));
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
        if (Bias.X > -(CurrentSizeDifference.X / 2.0)) {
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
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Left"));
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
        if (Bias.Y < CurrentSizeDifference.Y / 2.0) {
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
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Top"));
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
        if (Bias.Y > -(CurrentSizeDifference.Y / 2.0)) {
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
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Bottom"));
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

    //////////////////////////////////////////////if(CurrentDimensions.Min.Row < 0)

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
            UE_LOG(TileTablesOptimizationTools, Log, TEXT("old: %s, new %s"), *OldDimensions.ToString(), *CurrentDimensions.ToString());
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

                UE_LOG(TileTablesOptimizationTools, Log, TEXT("NewTopBoundMore: %d, OldTopBoundLess: %d, NewBottomBoundMore: %d, OldBottomBoundLess: %d, NewLeftBoundMore: %d, OldLeftBoundLess: %d, NewRightBoundMore: %d, OldRightBoundLess: %d"),
                    NewTopBoundMore, OldTopBoundLess, NewBottomBoundMore, OldBottomBoundLess, NewLeftBoundMore, OldLeftBoundLess, NewRightBoundMore, OldRightBoundLess);

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

                    //���� ���� ����������������, ��� ����� ������� ������� ���� ������� ������� ���������� �������, �� ��� ����������� ������ ������ ���������� ������������������
                    if (OldTopBoundLess) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is below the top of the previous area"));

                        if (!OldLeftBoundLess && !OldRightBoundLess) {
                            /* ��� ��� ��������, ��� ����� ������� ���� ����������, ���� �������� �� ������ �� 1 ����, ��� ������� �������
                             * �������, ����� ��������� ������ ����������� �����, � �������� �� ������ ������� ������� ������������ */
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

                    //���� ���� ����������������, ��� ����� ������� ������� ���� ������ ������� ���������� �������, �� ��� ����������� ����� ������ ���������� ��������
                    if (NewBottomBoundMore) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is below the lower boundary of the previous area"));

                        if (!NewLeftBoundMore && !NewRightBoundMore) {
                            /* ��� ��� ��������, ��� ����� ������� ���� ����������, ���� �������� �� ������ ������ ������� ������ �������, � ��������
                             * �� ������, ������������� �� 1 ����, ��� ������ ������ �������, ����� ��������� ������ ������������� ����� */
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

                    //���� ���� ����������������, ��� ����� ������� ������� ���� ������ ������� ���������� �������, �� ��� ����������� ������ ������ ���������� ������������������
                    if (OldBottomBoundLess) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is higher than the lower border of the previous area"));

                        if (!OldLeftBoundLess && !OldRightBoundLess) {
                            /* ��� ��� ��������, ��� ����� ������� ���� ����������, ���� �������� �� ������ ������ ������ ������ �������, �
                             * �������� �� ������, ������������� �� 1 ����, ��� ������� ������ �������, ����� ��������� ������ ����������� ����� */
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

                //���� ������ �������� �� X ������ ������������, �� ��� ��������, ��� ��� ����� �� ����������� � ��� ������ ����������, � ��� ������ ������ ��� �������� � �����
                if (!CurrentXLessThanMin) {
                    //���� ���� ����������������, ��� ����� ������� ������� ����� ����� ������� ���������� �������, �� ��� ����������� ����� ������� ���������� ��������
                    if (NewLeftBoundMore) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The current area is to the left of the left border of the previous area"));

                        AsynchronousAreaFilling(FGridDimensions(FGridCoord(CurrentDimensions.Min.Row, CurrentDimensions.Min.Col),
                            FGridCoord(CurrentDimensions.Max.Row, OldDimensions.Min.Col - 1)), NumberOfMapTilesRows, "3");
                    }

                    //���� ���� ����������������, ��� ����� ������� ������� ������ ����� ������� ���������� �������, �� ��� ����������� ������ ������� ���������� ������������������
                    if (OldLeftBoundLess) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is to the right of the left border of the previous area"));

                        /* ��� ��� ��������, ��� ����� ������� ������ ����������, ���� �������� �� ������� ������� ������ ����� �������, �
                         * �������� �� �������, �������������� �� 1 �����, ��� ������� ����� �������, ����� ��������� ������ ����������� ����� */
                        AsynchronousAreaRemoving(FGridDimensions(FGridCoord(OldDimensions.Min.Row, OldDimensions.Min.Col),
                            FGridCoord(OldDimensions.Max.Row, CurrentDimensions.Min.Col - 1)), NumberOfMapTilesRows, "3");
                    }

                    //���� ���� ����������������, ��� ����� ������� ������� ������ ������ ������� ���������� �������, �� ��� ����������� ����� ������� ���������� ��������
                    if (NewRightBoundMore) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is to the right of the right border of the previous area"));

                        /* ��� ��� ��������, ��� ����� ������� ������ ����������, ���� �������� �� ������� �� 1 ������, ��� ���������� ������
                         * �������, ����� ��������� ������ ������������� �����, � �������� �� ����� ������ ������� ������������ */
                        AsynchronousAreaFilling(FGridDimensions(FGridCoord(CurrentDimensions.Min.Row, OldDimensions.Max.Col + 1),
                            FGridCoord(CurrentDimensions.Max.Row, CurrentDimensions.Max.Col)), NumberOfMapTilesRows, "4");
                    }

                    //���� ���� ����������������, ��� ����� ������� ������� ����� ������ ������� ���������� �������, �� ��� ����������� ������ ������� ���������� ������������������
                    if (OldRightBoundLess) {
                        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                            UE_LOG(TileTablesOptimizationTools, Log, TEXT("TileTablesOptimizationTools class in the ChangingVisibilityOfTableTiles function: The new current area is to the left of the right border of the previous area"));

                        /* ��� ��� ��������, ��� ����� ������� ����� ����������, ���� �������� �� ������� �� 1 ������, ��� ������� ������
                         * �������, ����� ��������� ������ ����������� �����, � �������� �� ������ ������ ������� ������������ */
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

void UTileTablesOptimizationTools::ChangeDisplayAreaFromShift(FVector2D TileShift, int NumberOfMapTilesRows)
{
    if (!IsInit) {
        return;
    }

    int TileShiftRow = trunc(TileShift.Y);
    int TileShiftCol = trunc(TileShift.X);

    CurrentDimensions = OriginalDimensions + FGridDimensions(FGridCoord(TileShiftRow, TileShiftCol), FGridCoord(TileShiftRow, TileShiftCol));

    FGridDimensions OffsetDifference = CurrentDimensions - OldDimensions;

    if (OldDimensions != CurrentDimensions) {
        //UE_LOG(TileTablesOptimizationTools, Log, TEXT("old: %s, new %s"), *OldDimensions.ToString(), *CurrentDimensions.ToString());

        int MinRowCoordToFill = 0;
        int MaxRowCoordToFill = 0;
        int MinColCoordToFill = 0;
        int MaxColCoordToFill = 0;
        int MinRowCoordToRemove = 0;
        int MaxRowCoordToRemove = 0;
        int MinColCoordToRemove = 0;
        int MaxColCoordToRemove = 0;

        if (OldDimensions.DoTheDimensionsIntersect(CurrentDimensions)) {
            if (OffsetDifference.Max.Col > 0 && OffsetDifference.Max.Row == 0) {
                MinRowCoordToFill = CurrentDimensions.Min.Row;
                MaxRowCoordToFill = CurrentDimensions.Max.Row;

                MinRowCoordToRemove = CurrentDimensions.Min.Row;
                MaxRowCoordToRemove = CurrentDimensions.Max.Row;

                MinColCoordToFill = OldDimensions.Max.Col + 1;
                MaxColCoordToFill = CurrentDimensions.Max.Col;

                MinColCoordToRemove = OldDimensions.Min.Col;
                MaxColCoordToRemove = CurrentDimensions.Min.Col - 1;

                AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "pravo");
                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "pravo");
            }
            else if (OffsetDifference.Max.Col < 0 && OffsetDifference.Max.Row == 0) {
                MinRowCoordToFill = CurrentDimensions.Min.Row;
                MaxRowCoordToFill = CurrentDimensions.Max.Row;

                MinRowCoordToRemove = CurrentDimensions.Min.Row;
                MaxRowCoordToRemove = CurrentDimensions.Max.Row;

                MinColCoordToFill = CurrentDimensions.Min.Col;
                MaxColCoordToFill = OldDimensions.Min.Col - 1;

                MinColCoordToRemove = CurrentDimensions.Max.Col + 1;
                MaxColCoordToRemove = OldDimensions.Max.Col;

                AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "levo");
                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "levo");
            }
            else if (OffsetDifference.Max.Row > 0 && OffsetDifference.Max.Col == 0) {
                MinRowCoordToFill = OldDimensions.Max.Row + 1;
                MaxRowCoordToFill = CurrentDimensions.Max.Row;

                MinRowCoordToRemove = OldDimensions.Min.Row;
                MaxRowCoordToRemove = CurrentDimensions.Min.Row - 1;

                MinColCoordToFill = CurrentDimensions.Min.Col;
                MaxColCoordToFill = CurrentDimensions.Max.Col;

                MinColCoordToRemove = CurrentDimensions.Min.Col;
                MaxColCoordToRemove = CurrentDimensions.Max.Col;

                AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "verh");
                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "verh");
            }
            else if (OffsetDifference.Max.Row < 0 && OffsetDifference.Max.Col == 0) {
                MinRowCoordToFill = CurrentDimensions.Min.Row;
                MaxRowCoordToFill = OldDimensions.Min.Row - 1;

                MinRowCoordToRemove = CurrentDimensions.Max.Row + 1;
                MaxRowCoordToRemove = OldDimensions.Max.Row;

                MinColCoordToFill = CurrentDimensions.Min.Col;
                MaxColCoordToFill = CurrentDimensions.Max.Col;

                MinColCoordToRemove = CurrentDimensions.Min.Col;
                MaxColCoordToRemove = CurrentDimensions.Max.Col;

                AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "niz");
                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "niz");
            }
            else if (OffsetDifference.Max.Col > 0 && OffsetDifference.Max.Row > 0) {
                MinRowCoordToFill = OldDimensions.Max.Row + 1;
                MaxRowCoordToFill = CurrentDimensions.Max.Row;

                MinRowCoordToRemove = OldDimensions.Min.Row;
                MaxRowCoordToRemove = CurrentDimensions.Min.Row - 1;

                MinColCoordToFill = CurrentDimensions.Min.Col;
                MaxColCoordToFill = CurrentDimensions.Max.Col;

                MinColCoordToRemove = OldDimensions.Min.Col;
                MaxColCoordToRemove = OldDimensions.Max.Col;

                AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "pravo verh");
                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "pravo verh");

                MinRowCoordToFill = CurrentDimensions.Min.Row;
                MaxRowCoordToFill = OldDimensions.Max.Row;

                MinRowCoordToRemove = CurrentDimensions.Min.Row;
                MaxRowCoordToRemove = OldDimensions.Max.Row;

                MinColCoordToFill = OldDimensions.Max.Col + 1;
                MaxColCoordToFill = CurrentDimensions.Max.Col;

                MinColCoordToRemove = OldDimensions.Min.Col;
                MaxColCoordToRemove = CurrentDimensions.Min.Col - 1;

                AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "pravo verh");
                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "pravo verh");
            }
            else if (OffsetDifference.Max.Col > 0 && OffsetDifference.Max.Row < 0) {
                MinRowCoordToFill = CurrentDimensions.Min.Row;
                MaxRowCoordToFill = CurrentDimensions.Max.Row;

                MinRowCoordToRemove = OldDimensions.Min.Row;
                MaxRowCoordToRemove = OldDimensions.Max.Row;

                MinColCoordToFill = OldDimensions.Max.Col + 1;
                MaxColCoordToFill = CurrentDimensions.Max.Col;

                MinColCoordToRemove = OldDimensions.Min.Col;
                MaxColCoordToRemove = CurrentDimensions.Min.Col - 1;

                AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "pravo niz");
                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "pravo niz");

                MinRowCoordToFill = CurrentDimensions.Min.Row;
                MaxRowCoordToFill = OldDimensions.Min.Row - 1;

                MinRowCoordToRemove = CurrentDimensions.Max.Row + 1;
                MaxRowCoordToRemove = OldDimensions.Max.Row;

                MinColCoordToFill = CurrentDimensions.Min.Col;
                MaxColCoordToFill = OldDimensions.Max.Col;

                MinColCoordToRemove = CurrentDimensions.Min.Col;
                MaxColCoordToRemove = OldDimensions.Max.Col;

                AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "pravo niz");
                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "pravo niz");
            }
            else if (OffsetDifference.Max.Col < 0 && OffsetDifference.Max.Row > 0) {
                MinRowCoordToFill = OldDimensions.Max.Row + 1;
                MaxRowCoordToFill = CurrentDimensions.Max.Row;

                MinRowCoordToRemove = OldDimensions.Min.Row;
                MaxRowCoordToRemove = CurrentDimensions.Min.Row - 1;

                MinColCoordToFill = CurrentDimensions.Min.Col;
                MaxColCoordToFill = CurrentDimensions.Max.Col;

                MinColCoordToRemove = OldDimensions.Min.Col;
                MaxColCoordToRemove = OldDimensions.Max.Col;

                AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "levo verh");
                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "levo verh");

                MinRowCoordToFill = CurrentDimensions.Min.Row;
                MaxRowCoordToFill = OldDimensions.Max.Row;

                MinRowCoordToRemove = CurrentDimensions.Min.Row;
                MaxRowCoordToRemove = OldDimensions.Max.Row;

                MinColCoordToFill = CurrentDimensions.Min.Col;
                MaxColCoordToFill = OldDimensions.Min.Col - 1;

                MinColCoordToRemove = CurrentDimensions.Max.Col + 1;
                MaxColCoordToRemove = OldDimensions.Max.Col;

                AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "levo verh");
                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "levo verh");
            }
            else if (OffsetDifference.Max.Col < 0 && OffsetDifference.Max.Row < 0) {
                MinRowCoordToFill = CurrentDimensions.Min.Row;
                MaxRowCoordToFill = OldDimensions.Min.Row - 1;

                MinRowCoordToRemove = CurrentDimensions.Max.Row + 1;
                MaxRowCoordToRemove = OldDimensions.Max.Row;

                MinColCoordToFill = CurrentDimensions.Min.Col;
                MaxColCoordToFill = CurrentDimensions.Max.Col;

                MinColCoordToRemove = OldDimensions.Min.Col;
                MaxColCoordToRemove = OldDimensions.Max.Col;

                AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "levo niz");
                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "levo niz");

                MinRowCoordToFill = OldDimensions.Min.Row;
                MaxRowCoordToFill = CurrentDimensions.Max.Row;

                MinRowCoordToRemove = OldDimensions.Min.Row;
                MaxRowCoordToRemove = CurrentDimensions.Max.Row;

                MinColCoordToFill = CurrentDimensions.Min.Col;
                MaxColCoordToFill = OldDimensions.Min.Col - 1;

                MinColCoordToRemove = CurrentDimensions.Max.Col + 1;
                MaxColCoordToRemove = OldDimensions.Max.Col;

                AsynchronousAreaFilling(FGridDimensions(FGridCoord(MinRowCoordToFill, MinColCoordToFill), FGridCoord(MaxRowCoordToFill, MaxColCoordToFill)), NumberOfMapTilesRows, "levo niz");
                AsynchronousAreaRemoving(FGridDimensions(FGridCoord(MinRowCoordToRemove, MinColCoordToRemove), FGridCoord(MaxRowCoordToRemove, MaxColCoordToRemove)), NumberOfMapTilesRows, "levo niz");
            }
        }
        else {
            AsynchronousAreaFilling(CurrentDimensions, NumberOfMapTilesRows, "all");
            AsynchronousAreaRemoving(OldDimensions, NumberOfMapTilesRows, "all");
        }
        TilesCoordWrapper->setMinCoord(CurrentDimensions.Min);
        TilesCoordWrapper->setMaxCoord(CurrentDimensions.Max);
    }

    OldDimensions = CurrentDimensions;
}


void UTileTablesOptimizationTools::ChangeDisplayAreaFromZoom(float TileZoom, int NumberOfMapTilesRows) {
    //if (!IsInit) {
    //    return;
    //}

    //if (!TilesCoordWrapper)
    //{
    //    return;
    //}
    //
    //if (OldAreaSize == FVector2D(0, 0))
    //    OldAreaSize = AreaSize;

    //FGridCoord MaxCoord = TilesCoordWrapper->getMaxCoord();
    //FGridCoord MinCoord = TilesCoordWrapper->getMinCoord();
    //FVector2D DimensionsSize = FVector2D((MaxCoord.Col - MinCoord.Col) * TileSize.X, (MaxCoord.Row - MinCoord.Row) * TileSize.Y);

    //SizeDifference = OldAreaSize - DimensionsSize * TileZoom;

    //int DifTilesCols = ceil((SizeDifference.X) / TileSize.X);
    //int DifTilesRows = ceil((SizeDifference.Y) / TileSize.Y);

    //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("sss Col: %d Row: %d"), DifTilesCols, DifTilesRows));
}