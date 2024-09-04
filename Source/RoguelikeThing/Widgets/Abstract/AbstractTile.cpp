// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Widgets/Abstract/AbstractTile.h"

void UAbstractTile::SetMyCoord(FCellCoord myCoord)
{
}

const FCellCoord UAbstractTile::GetMyCoord()
{
    return FCellCoord();
}

//Виртуальная функция для сброса состояния изменённых дочерних виджетов (если таковые имеются)
void UAbstractTile::ClearFilledCells()
{
}

//Виртуальная функция для удаления всех дочерних виджетов (если таковые имеются)
void UAbstractTile::RemoveAllCells()
{
}

//Эвент, вызываемый при добавлении тайла в таблицу
void UAbstractTile::OnAddedEvent()
{
}

//Альтернативный эвент, вызываемый при добавлении тайла в таблицу, с классом взаимодействия с БД в качестве аргумента
void UAbstractTile::OnAddedEvent(UMapMatrix* Map)
{
}
