// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Widgets/MapEditor/MapCell.h"

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

FCellCoord UMapCell::GetMyGlobalCoord()
{
    return FCellCoord(CoordOfParentTile->Row * MapTileLength + MyCoord.Row, CoordOfParentTile->Col * MapTileLength + MyCoord.Col);
}
