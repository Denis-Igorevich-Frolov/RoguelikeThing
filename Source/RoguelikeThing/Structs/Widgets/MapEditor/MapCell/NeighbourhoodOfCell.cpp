// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Structs/Widgets/MapEditor/MapCell/NeighbourhoodOfCell.h"

/* ����������� �� ��������� ������ ����������, � ������ IsInit ����� false. ����� ��������� ����� ���������
 * �����������, �� ������� ��������� � ����� ����� ���������� ���������� IsInit �������� true ������� */
FNeighbourhoodOfCell::FNeighbourhoodOfCell() : IsInit(false)
{
}

FNeighbourhoodOfCell::FNeighbourhoodOfCell(bool SomethingOnRight, bool SomethingOnLeft, bool SomethingOnTop, bool SomethingOnBottom) :
    SomethingOnRight(SomethingOnRight), SomethingOnLeft(SomethingOnLeft), SomethingOnTop(SomethingOnTop), SomethingOnBottom(SomethingOnBottom), IsInit(true)
{
}