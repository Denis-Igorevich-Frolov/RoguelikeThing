// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Widgets/MapEditor/MapCell.h"
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(MapCell);

UMapCell::UMapCell(const FObjectInitializer& Object) : UAbstractTile(Object)
{
    //Получение GameInstance из мира
    GameInstance = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    if (!GameInstance)
        UE_LOG(MapCell, Warning, TEXT("Warning in MapCell class in constructor - GameInstance was not retrieved from the world"));
}

void UMapCell::SetCoordOfParentTile(FCellCoord& coordOfParentTile, int mapTileLength)
{
    this->CoordOfParentTile = &coordOfParentTile;
    this->MapTileLength = mapTileLength;
}

FCellCoord UMapCell::GetCoordOfParentTile()
{
    return *CoordOfParentTile;
}

void UMapCell::SetMyCoord(FCellCoord myCoord)
{
    this->MyCoord = myCoord;
}

/* Функция, возвращающая глобальную координату ячейки, то есть с учётом координаты родительского тайла. Так, например,
 * ячейка 0:1, находящаяся в тайле 0:3, при длине стороны тайла в 5, будет имень глобальную координату 0:16 */
FCellCoord UMapCell::GetMyGlobalCoord()
{
    return FCellCoord(CoordOfParentTile->Row * MapTileLength + MyCoord.Row, CoordOfParentTile->Col * MapTileLength + MyCoord.Col);
}

//Функция, возвращающая координату ячейки внутри своего родительского тайла
FCellCoord UMapCell::GetMyLocalCoord()
{
    return MyCoord;
}
