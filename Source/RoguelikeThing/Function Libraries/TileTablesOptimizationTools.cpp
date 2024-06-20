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

void UTileTablesOptimizationTools::Init(UCoordWrapperOfTable* tilesCoordWrapper, UTileBuffer* tilesBuf, FGridDimensions originalDimensions)
{
    this->TilesCoordWrapper = tilesCoordWrapper;
    this->TilesBuf = tilesBuf;

    this->OriginalDimensions = originalDimensions;
    OldDimensions = OriginalDimensions;
    CurrentDimensions = OriginalDimensions;
}

void UTileTablesOptimizationTools::ChangeDisplayAreaFromShift(FVector2D TileShift)
{
    if (!IsInit) {
        return;
    }

    int TileShiftRow = trunc(TileShift.Y);
    int TileShiftCol = trunc(TileShift.X);

    CurrentDimensions = OriginalDimensions + FGridDimensions(FGridCoord(0,0), FGridCoord(TileShiftRow, TileShiftCol));

    if (OldDimensions != CurrentDimensions) {

    }

    OldDimensions = CurrentDimensions;
}
