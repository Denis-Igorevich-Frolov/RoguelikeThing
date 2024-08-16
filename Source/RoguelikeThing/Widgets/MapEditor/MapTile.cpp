// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Widgets/MapEditor/MapTile.h"
#include "RoguelikeThing/Widgets/MapEditor/MapEditor.h"
#include <RoguelikeThing/Enumerations/MapEditorBrushType.h>
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(Map_Tile);

UMapTile::UMapTile(const FObjectInitializer& Object) : UAbstractTile(Object)
{
    FString Name("CellsCoordWrapper_");
    Name.Append(GetName());
    CellsCoordWrapper = CreateDefaultSubobject<UCoordWrapperOfTable>(*Name);

    //��������� GameInstance �� ����
    GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (!GameInstance)
        UE_LOG(Map_Tile, Warning, TEXT("Warning in MapTile class in constructor - GameInstance was not retrieved from the world"));
}

UMapTile::~UMapTile()
{
    FilledCells.Empty();
}

//��������������� ����������� ������� ��� �������� ���� �����
void UMapTile::RemoveAllCells()
{
    if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
        UE_LOG(Map_Tile, Log, TEXT("MapTile class in the RemoveAllCells function: Deleting all cells in the tile has begun"));
    if (CellsCoordWrapper) {
        CellsCoordWrapper->Clear();

        if (CellsCoordWrapper->IsValidLowLevel()) {
            CellsCoordWrapper->ConditionalBeginDestroy();
            CellsCoordWrapper->MarkPendingKill();
        }
        else {
            UE_LOG(Map_Tile, Error, TEXT("!!! An error occurred in the MapTile class in the RemoveAllCells function: CellsCoordWrapper is not valid low level"));
        }
    }
    else {
        UE_LOG(Map_Tile, Error, TEXT("!!! An error occurred in the MapTile class in the RemoveAllCells function: CellsCoordWrapper is not valid"));
    }
}

void UMapTile::SetMyCoord(FCellCoord myCoord)
{
    this->MyCoord = myCoord;
}

void UMapTile::SetMyTerrainOfTile(UTerrainOfTile* TerrainOfTile)
{
    MyTerrainOfTile = TerrainOfTile;
}

UCoordWrapperOfTable* UMapTile::GetCellsCoordWrapper()
{
    return CellsCoordWrapper;
}

//�������, ����������� ���� ��������. ���� MapMatrix ����� �������, �� � ����� ����� ����������� ��� ����������� �����, ����� ��� ������ ����� �� ������ �� ���������
bool UMapTile::FillingWithCells(int MapTileLength, UClass* CellClass, UMapEditor* MapEditor, UMapMatrix* MapMatrix)
{
    if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
        UE_LOG(Map_Tile, Log, TEXT("MapTile class in the FillingWithCells function: Filling of the tile col: %d row: %d with cells has begun"), MyCoord.Col, MyCoord.Row);

    /* ���� ���������� �������� �� ����� ������ �� ��������, ��� � ����� ������� -
     * ������ ������ ��������� ����� �����, � ��������� - ������ ������ */
    for (int row = MapTileLength - 1; row >= 0; row--) {
        for (int col = 0; col < MapTileLength; col++) {
            UPROPERTY()
            UMapCell* Cell = CreateWidget<UMapCell>(this, CellClass);

            if (!Cell) {
                UE_LOG(Map_Tile, Error, TEXT("!!! An error occurred in the MapTile class in the FillingWithCells function: cell Col: %d, row: %d was created with an error"), col, row);
                return false;
            }

            //������ ��������� ���������� ����� � � ����������� ����������
            Cell->SetCoordOfParentTile(MyCoord, MapTileLength);
            Cell->SetMyCoord(FCellCoord(row, col));
            //����� �� ��������� ��������� �� � �������� ����
            Cell->MyEditor = MapEditor;

            if (GetGridPanel()) {
                //������������ ���� ������ ��� ����, ����� �� ��� ������� � ������������ ������
                UUniformGridSlot* GridSlot = GetGridPanel()->AddChildToUniformGrid(Cell, row, col);

                if (GridSlot) {
                    if (CellsCoordWrapper) {
                        /* ��������� ������ ���������� � ������������ ������. ��� ���� ����� �� ������ � �������
                         * ��������, �����, ��������, ����� ����� ���� ������ 0x0, � ������ ������ - 5x5.
                         * ��� ����� ������ ��������� ����� �� ������� (���� ��� ������ ����������� � row--)
                         * ���������������, ����� �������� ��������������� �������� �� ����, ��� ���� ���
                         * ������������� ����� ��� ����������� ������������ ����� � GridPanel */
                        CellsCoordWrapper->AddWidget(MapTileLength - 1 - row, col, Cell, GridSlot);
                    }
                    else {
                        UE_LOG(Map_Tile, Error, TEXT("!!! An error occurred in the MapTile class in the FillingWithCells function: CellsCoordWrapper is not valid"));
                        return false;
                    }
                }
                else {
                    UE_LOG(Map_Tile, Error, TEXT("!!! An error occurred in the MapTile class in the FillingWithCells function: GridSlot of cell is not valid"));
                    return false;
                }
            }
            else
                UE_LOG(Map_Tile, Error, TEXT("!!! An error occurred in the MapTile class in the FillingWithCells function: Failed to get GridPanel from MapTile col: %d, row: %d"), MyCoord.Col, MyCoord.Row);

            //������ ������� ����� ������ �� ���������� ������������ MyTerrainOfTile
            SetStyleFromTerrainOfTile(Cell, row, col, MapTileLength, MapMatrix);

            if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                UE_LOG(Map_Tile, Log, TEXT("MapTile class in the FillingWithCells function: Cell col: %d row: %d is fully created"), col, row);

        }
    }

    if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
        UE_LOG(Map_Tile, Log, TEXT("MapTile class in the FillingWithCells function: The tile col: %d row: %d is completely filled with cells"), MyCoord.Col, MyCoord.Row);

    return true;
}

//�������, ����������� ���������� � �������, ����� ������� ���������� �� ��������
void UMapTile::UpdateInformationAboutCells(UMapCell* Cell, FMapEditorBrushType CellStyle)
{
    if (!Cell) {
        UE_LOG(Map_Tile, Error, TEXT("!!! An error occurred in the MapTile class in the UpdateInformationAboutCells function: Transmitted cell is not valid"));
        return;
    }

    if (GameInstance && GameInstance->LogType == ELogType::DETAILED) {
        const UEnum* CellType = FindObject<UEnum>(ANY_PACKAGE, TEXT("FMapEditorBrushType"), true);

        UE_LOG(Map_Tile, Log, TEXT("MapTile class in the UpdateInformationAboutCells function: Cell col: %d row: %d style changed to %s"), Cell->GetMyGlobalCoord().Col, Cell->GetMyGlobalCoord().Row, *(CellType ? CellType->GetEnumName((int32)CellStyle) : TEXT("<Invalid Enum>")));
    }

    //���� ����� ������ �� ������, �� �� ������������ � ������ ����������� �����
    if ((CellStyle == FMapEditorBrushType::Corridor) || (CellStyle == FMapEditorBrushType::Room)) {
        if (!FilledCells.Contains(Cell)) {
            FilledCells.Add(Cell);
        }

        if (MyTerrainOfTile) {
            //� ���������� ������������ ����� �������� ��������� ��������� �����
            MyTerrainOfTile->AddCellType(Cell->GetMyLocalCoord(), CellStyle);
        }
        else {
            UE_LOG(Map_Tile, Error, TEXT("!!! An error occurred in the MapTile class in the UpdateInformationAboutCells function: MyTerrainOfTile is not valid"));
        }
    }
    //�����, ���� ������ ����� �� ��� �������, � ����� ����� ������� ����, �� �� ��������� �� ������� ����������� �����
    else {
        if (FilledCells.Contains(Cell)) {
            FilledCells.Remove(Cell);
        }

        if (MyTerrainOfTile) {
            //� ���������� ������������ ����� �������� ��������� ��������� �����
            FCellCoord CellCoord = Cell->GetMyLocalCoord();
            if (MyTerrainOfTile->Contains(CellCoord)) {
                MyTerrainOfTile->RemoveCell(CellCoord);
            }
        }
        else {
            UE_LOG(Map_Tile, Error, TEXT("!!! An error occurred in the MapTile class in the UpdateInformationAboutCells function: MyTerrainOfTile is not valid"));
        }
    }
}

//�������, ���������� ����� ����� � ������������ � ���������� ������������ MyTerrainOfTile
bool UMapTile::FillCellsAccordingToTerrain(UMapMatrix* MapMatrix)
{
    if (MyTerrainOfTile) {
        TArray<FCellCoord> FilledCoord = MyTerrainOfTile->GetFilledCoord();
        for (FCellCoord CellCoord : FilledCoord) {
            //��������� ��������� �� ������ �����. ������ ��������� ������ ��� � �� ���������� ���������� ��������� � ����� ������� ����, � � UI - � ����� ������
            UAbstractTile* AbstractCell = CellsCoordWrapper->FindWidget(MapMatrix->GetMapTileLength() - 1 - CellCoord.Row, CellCoord.Col);

            if (AbstractCell) {
                if (dynamic_cast<UMapCell*>(AbstractCell)) {
                    UMapCell* Cell = static_cast<UMapCell*>(AbstractCell);

                    if (Cell) {
                        FMapEditorBrushType CellStyle = MyTerrainOfTile->GetCellType(CellCoord);

                        if (CellStyle != FMapEditorBrushType::Error) {
                            FCellCoord CellGlobalCoord = Cell->GetMyGlobalCoord();
                            switch (CellStyle)
                            {
                            case FMapEditorBrushType::Corridor:
                                Cell->SetCorridorStyle(MapMatrix->CheckNeighbourhoodOfCell(MatrixType::ChunkStructure, CellGlobalCoord.Row, CellGlobalCoord.Col));
                                FilledCells.Add(Cell);

                                if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                                    UE_LOG(Map_Tile, Log, TEXT("Map_Tile class in the FillCellsAccordingToTerrain function: Cell row: %d col: %d is set to the corridor style"), CellGlobalCoord.Row, CellGlobalCoord.Col);
                                break;
                            case FMapEditorBrushType::Room:
                                Cell->SetRoomStyle();
                                FilledCells.Add(Cell);

                                if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                                    UE_LOG(Map_Tile, Log, TEXT("Map_Tile class in the FillCellsAccordingToTerrain function: Cell row: %d col: %d is set to the room style"), CellGlobalCoord.Row, CellGlobalCoord.Col);
                                break;
                            default:
                                break;
                            }
                        }
                        else {
                            UE_LOG(Map_Tile, Error, TEXT("!!! An error occurred in the MapTile class in the FillCellsAccordingToTerrain function: CellStyle equal to Error"));
                            return false;
                        }
                    }
                    else {
                        UE_LOG(Map_Tile, Error, TEXT("!!! An error occurred in the MapTile class in the FillCellsAccordingToTerrain function: Cell is not valid"));
                        return false;
                    }
                }
                else {
                    UE_LOG(Map_Tile, Error, TEXT("!!! An error occurred in the MapTile class in the FillCellsAccordingToTerrain function: AbstractCell is not UMapCell"));
                    return false;
                }
            }
            else {
                UE_LOG(Map_Tile, Error, TEXT("!!! An error occurred in the MapTile class in the FillCellsAccordingToTerrain function: AbstractCell is not valid"));
                return false;
            }
        }
    }
    else {
        UE_LOG(Map_Tile, Error, TEXT("!!! An error occurred in the MapTile class in the FillCellsAccordingToTerrain function: MyTerrainOfTile is not valid"));
        return false;
    }

    return true;
}

//�������, �������� ����� ����� ������ �� ���������� ������������ MyTerrainOfTile
void UMapTile::SetStyleFromTerrainOfTile(UMapCell* Cell, int row, int col, int MapTileLength, UMapMatrix* MapMatrix)
{
    //��� ����� �������� ������ � GameThread, ������ ��� � �� ��� ��� ������� ����������
    AsyncTask(ENamedThreads::GameThread, [Cell, row, col, MapTileLength, MapMatrix, this]() {
        if (MapMatrix) {
            int GlobalRow = MyCoord.Row * MapTileLength + row;
            int GlobalCol = MyCoord.Col * MapTileLength + col;

            //������ ������ ����� ������� ����� ������ ����������� �����. ��� ����� �� �� �������� ��� ��������� ������
            FMapEditorBrushType CellType = MapMatrix->GetCellStyleFromTerrainOfTile(FCellCoord(GlobalRow, GlobalCol), MapTileLength);

            /* � ����� � ������������ � ��������� �����, ������ ������������� ����������� �����.
             * ��� ���� ������ ����� ��������� �� ����, �� � ��� �������� ������ �� ��������� */
            switch (CellType)
            {
            case FMapEditorBrushType::Corridor:
                Cell->SetCorridorStyle(MapMatrix->CheckNeighbourhoodOfCell(MatrixType::ChunkStructure, GlobalRow, GlobalCol));
                if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                    UE_LOG(Map_Tile, Log, TEXT("MapTile class in the SetStyleFromTerrainOfTile function: Cell row: %d col: %d is set to the corridor style"), GlobalRow, GlobalCol);
                FilledCells.Add(Cell);
                break;
            case FMapEditorBrushType::Room:
                Cell->SetRoomStyle();
                if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                    UE_LOG(Map_Tile, Log, TEXT("MapTile class in the SetStyleFromTerrainOfTile function: Cell row: %d col: %d is set to the room style"), GlobalRow, GlobalCol);
                FilledCells.Add(Cell);
                break;
            default:
                break;
            }
        }
        });
}

//��������������� ����������� ������� ��� ������ ��������� ��������� �����
void UMapTile::ClearFilledCells()
{
    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [this]() {
        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(Map_Tile, Log, TEXT("MapTile class in the ClearFilledCells function: A thread is started in the tile col: %d, row: %d to reset the state of all filled cells"), MyCoord.Col, MyCoord.Row);
        
        if (FilledCells.Num() != 0) {
            for (UMapCell* Cell : FilledCells) {
                //�������� ����� ������� �������� ������ � �������� ������
                AsyncTask(ENamedThreads::GameThread, [Cell]() {
                    Cell->SetEmptinessStyle();
                    });
            }

            /* ��� ������� ������� ������� ����� ������ ���������� ����������� �������� ���������.
             * � ��� ��� ��� ������ ������ ���������� �� ������, ��������� ��� ���������� �� �������,
             * � �� ��� ��������, �� ������� �������� �� ���������� �������� ��� ������� ������� */
            FilledCells = TArray<UMapCell*>();
        }

        MyTerrainOfTile = nullptr;

        if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
            UE_LOG(Map_Tile, Log, TEXT("MapTile class in the ClearFilledCells function: A thread is closed in the tile col: %d, row: %d to reset the state of all filled cells"), MyCoord.Col, MyCoord.Row);
        });
}

//��������������� ����������� �������, ���������� ��� ���������� ����� � �������
void UMapTile::OnAddedEvent(UMapMatrix* MapMatrix)
{
    SetMyTerrainOfTile(MapMatrix->GetTerrainOfTile(MyCoord));
    FillCellsAccordingToTerrain(MapMatrix);
}
