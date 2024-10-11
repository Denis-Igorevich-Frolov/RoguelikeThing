// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Widgets/MapEditor/CellCoord.h"

FCellCoord::FCellCoord(int32 Row, int32 Col) : Row(Row), Col(Col)
{
}

bool FCellCoord::operator==(const FCellCoord& Coord) const
{
    return (this->Row == Coord.Row) && (this->Col == Coord.Col);
}

bool FCellCoord::operator!=(const FCellCoord& Coord) const
{
    return (this->Row != Coord.Row) || (this->Col != Coord.Col);
}
