// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Widgets/MapEditor/MapTile.h"
#include "RoguelikeThing/Widgets/MapEditor/MapEditor.h"
#include <RoguelikeThing/Enumerations/MapEditorBrushType.h>
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(Map_Tile);

UMapTile::UMapTile(const FObjectInitializer& Object) : UAbstractTile(Object)
{
    CellsCoordWrapper = NewObject<UCoordWrapperOfTable>();

    //Получение GameInstance из мира
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

bool UMapTile::FillingWithCells(int MapTileLength, UClass* CellClass, UMapEditor* MapEditor, UMapMatrix* MapMatrix)
{
    /* Тайл забивается ячейками по точно такому же принципу, что и карта тайлами -
     * первая ячейка находится слева снизу, а последняя - справа сверху */
    for (int row = MapTileLength - 1; row >= 0; row--) {
        for (int col = 0; col < MapTileLength; col++) {
            UMapCell* Cell = CreateWidget<UMapCell>(this, CellClass);

            if (!Cell) {
                UE_LOG(Map_Tile, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: Cell was created with an error"));

                return false;
            }

            Cell->SetCoordOfParentTile(MyCoord, MapTileLength);
            Cell->SetMyCoord(FCellCoord(row, col));
            //Также ей передаётся указатель на её редактор карт
            Cell->MyEditor = MapEditor;

            //if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                //UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: An uninitialized cell is created with coordinates row: %d col: %d for a MapTile with coordinates row: %d col: %d"), row, col, row, col);

            //Добавление созданной ячейки в GridPanel производится в основном потоке так как сделать это вне его невозможно
            AsyncTask(ENamedThreads::GameThread, [Cell, row, col, MapTileLength, MapMatrix, this]() {
                //if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                    //UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Launched a GameThread call from the TilesGridPanel table population thread to place a cell in a tile"));

                if (GetGridPanel()) {
                    UUniformGridSlot* GridSlot = GetGridPanel()->AddChildToUniformGrid(Cell, row, col);

                    /* Созданные ячейки забиваются в координатную обёртку. При этом важен их индекс в порядке
                     * создания, чтобы, например, слева снизу была ячейка 0x0, а справа сверху - 5x5.
                     * Для этого индекс обратного цикла по строкам разворачивается, чтобы передать
                     * инвертированное значение от того, что было уже инвертировано для корректного
                     * расположения ячеек в GridPanel */
                    CellsCoordWrapper->AddWidget(MapTileLength - 1 - row, col, Cell, GridSlot);
                    //if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                        //UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: The cell was placed in a tile located at coordinates col: %d row: %d, at position col: %d row: %d"), row, col, row, col);
                }
                //else
                    //UE_LOG(FillingMapWithCells, Error, TEXT("!!! An error occurred in the FillingMapWithCells class in the FillMapEditorWithCells function: Failed to get GridPanel from MapTile"));

                //if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                    //UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: A thread to place a cell in a tile has been closed"));
                
                SetStyleFromDB(Cell, row, col, MapTileLength, MapMatrix);
                });

        }
    }

    return true;
}

void UMapTile::SetStyleFromDBForAllCells(int MapTileLength, UMapMatrix* MapMatrix)
{
    AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [MapTileLength, MapMatrix, this]() {
        if (CellsCoordWrapper && CellsCoordWrapper->HasAnyEllements()) {
            FGridCoord MinCoord = CellsCoordWrapper->getMinCoord();
            FGridCoord MaxCoord = CellsCoordWrapper->getMaxCoord();

            for (int row = MinCoord.Row; row <= MaxCoord.Row; row++) {
                for (int col = MinCoord.Col; col <= MaxCoord.Col; col++) {
                    UAbstractTile* AbstractCell = CellsCoordWrapper->FindWidget(row, col);

                    if (dynamic_cast<UMapCell*>(AbstractCell)) {
                        UMapCell* Cell = static_cast<UMapCell*>(AbstractCell);

                        SetStyleFromDB(Cell, MapTileLength - 1 - row, col, MapTileLength, MapMatrix);
                    }
                }
            }
        }
        });
}

void UMapTile::AddFilledCell(UMapCell* Cell)
{
    FilledCells.Add(Cell);
}

void UMapTile::SetStyleFromDB(UMapCell* Cell, int row, int col, int MapTileLength, UMapMatrix* MapMatrix)
{
    AsyncTask(ENamedThreads::GameThread, [Cell, row, col, MapTileLength, MapMatrix, this]() {
        if (MapMatrix) {
            //Теперь пришло время придать новой ячейке необходимый стиль. Для этого из БД читается тип структуры ячейки
            FMapEditorBrushType CellType = MapMatrix->GetValueOfMapChunkStructureCellByGlobalIndex(MyCoord.Row * MapTileLength + row, MyCoord.Col * MapTileLength + col, false);

            /* И затем в соответствии с полученым типом, ячейке присваивается необходимый стиль.
             * При этом пустой стиль назначать не надо, он и так является стилем по умолчанию */
            switch (CellType)
            {
            case FMapEditorBrushType::Corridor:
                Cell->SetCorridorStyle(MapMatrix->CheckNeighbourhoodOfCell(MatrixType::ChunkStructure, MyCoord.Row * MapTileLength + row, MyCoord.Col * MapTileLength + col, false));
                //if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                    //UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Cell Row: %d Col: %d is set to the corridor style"), row * MapTileLength + row + 1, col * MapTileLength + col + 1);
                FilledCells.Add(Cell);
                break;
            case FMapEditorBrushType::Room:
                Cell->SetRoomStyle();
                //if (GameInstance && GameInstance->LogType == ELogType::DETAILED)
                    //UE_LOG(FillingMapWithCells, Log, TEXT("FillingMapWithCells class in the FillMapEditorWithCells function: Cell Row: %d Col: %d is set to the room style"), row * MapTileLength + row + 1, col * MapTileLength + col + 1);
                FilledCells.Add(Cell);
                break;
            default:
                break;
            }
        }
        });
}

void UMapTile::ClearFilledCells()
{
    if (FilledCells.Num() != 0) {
        for (UMapCell* Cell : FilledCells) {
            Cell->SetEmptinessStyle();
        }

        FilledCells = TArray<UMapCell*>();
    }
}

void UMapTile::OnAddedEvent(int MapTileLength, UMapMatrix* MapMatrix)
{
    SetStyleFromDBForAllCells(MapTileLength, MapMatrix);
}
