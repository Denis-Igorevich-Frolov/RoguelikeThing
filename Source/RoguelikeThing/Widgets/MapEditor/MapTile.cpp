// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Widgets/MapEditor/MapTile.h"
#include "RoguelikeThing/Widgets/MapEditor/MapEditor.h"
#include "MapCell.h"
#include <RoguelikeThing/Enumerations/MapEditorBrushType.h>
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(Map_Tile);

UMapTile::UMapTile(const FObjectInitializer& Object) : UAbstractTile(Object)
{
    CellsCoordWrapper = NewObject<UCoordWrapperOfTable>();

    //��������� GameInstance �� ����
    GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (!GameInstance)
        UE_LOG(Map_Tile, Warning, TEXT("Warning in MapTile class in constructor - GameInstance was not retrieved from the world"));
}

void UMapTile::SetMyCoord(FCellCoord myCoord)
{
    this->MyCoord = myCoord;
}

UCoordWrapperOfTable* UMapTile::GetCellsCoordWrapper()
{
    return CellsCoordWrapper;
}

bool UMapTile::FillingWithCells(int MapTileLength, UClass* CellClass, UMapEditor* MapEditor)
{
    /* ���� ���������� �������� �� ����� ������ �� ��������, ��� � ����� ������� -
     * ������ ������ ��������� ����� �����, � ��������� - ������ ������ */
    for (int tileRow = MapTileLength - 1; tileRow >= 0; tileRow--) {
        for (int tileCol = 0; tileCol < MapTileLength; tileCol++) {
            UMapCell* Cell = CreateWidget<UMapCell>(this, CellClass);

            if (!Cell) {
                UE_LOG(Map_Tile, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: Cell was created with an error"));

                return false;
            }

            Cell->SetCoordOfParentTile(MyCoord, MapTileLength);
            Cell->SetMyCoord(FCellCoord(tileRow, tileCol));
            //����� �� ��������� ��������� �� � �������� ����
            Cell->MyEditor = MapEditor;

            //if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                //UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: An uninitialized cell is created with coordinates row: %d col: %d for a MapTile with coordinates row: %d col: %d"), tileRow, tileCol, row, col);

            //���������� ��������� ������ � GridPanel ������������ � �������� ������ ��� ��� ������� ��� ��� ��� ����������
            AsyncTask(ENamedThreads::GameThread, [Cell, tileRow, tileCol, MapTileLength, this]() {
                //if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                    //UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Launched a GameThread call from the TilesGridPanel table population thread to place a cell in a tile"));

                if (GetGridPanel()) {
                    UUniformGridSlot* GridSlot = GetGridPanel()->AddChildToUniformGrid(Cell, tileRow, tileCol);

                    /* ��������� ������ ���������� � ������������ ������. ��� ���� ����� �� ������ � �������
                     * ��������, �����, ��������, ����� ����� ���� ������ 0x0, � ������ ������ - 5x5.
                     * ��� ����� ������ ��������� ����� �� ������� ���������������, ����� ��������
                     * ��������������� �������� �� ����, ��� ���� ��� ������������� ��� �����������
                     * ������������ ����� � GridPanel */
                    CellsCoordWrapper->AddWidget(MapTileLength - 1 - tileRow, tileCol, Cell, GridSlot);
                    //if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                        //UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The cell was placed in a tile located at coordinates col: %d row: %d, at position col: %d row: %d"), tileRow, tileCol, row, col);
                }
                //else
                    //UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: Failed to get GridPanel from MapTile"));

                //if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                    //UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: A thread to place a cell in a tile has been closed"));

                });

            //������ ������ ����� ������� ����� ������ ����������� �����. ��� ����� �� �� �������� ��� ��������� ������
            //FMapEditorBrushType CellType = MapMatrix->GetValueOfMapChunkStructureCellByGlobalIndex(row * MapTileLength + tileRow + 1, col * MapTileLength + tileCol + 1, false);

            /* � ����� � ������������ � ��������� �����, ������ ������������� ����������� �����.
             * ��� ���� ������ ����� ��������� �� ����, �� � ��� �������� ������ �� ��������� */
            /*switch (CellType)
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
            }*/
        }
    }

    return true;
}
