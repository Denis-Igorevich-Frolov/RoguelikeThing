// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Structs/Widgets/MapEditor/MapCell/NeighbourhoodOfCell.h"

/*  онструктор по умолчанию делает экземпл€ры, у кторых IsInit равна false. “акие структуры будут считатьс€
 * невалидными, их следует заполнить и после этого установить переменной IsInit значение true вручную */
FNeighbourhoodOfCell::FNeighbourhoodOfCell() : IsInit(false)
{
}

FNeighbourhoodOfCell::FNeighbourhoodOfCell(bool SomethingOnRight, bool SomethingOnLeft, bool SomethingOnTop, bool SomethingOnBottom) :
    SomethingOnRight(SomethingOnRight), SomethingOnLeft(SomethingOnLeft), SomethingOnTop(SomethingOnTop), SomethingOnBottom(SomethingOnBottom), IsInit(true)
{
}