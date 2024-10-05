// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Map/MapDimensions.h"

FMapDimensions::FMapDimensions(int32 MinRow, int32 MaxRow, int32 MinCol,
    int32 MaxCol, int32 TableLength, int32 MapTileLength, bool isValid) :
    MinRow(MinRow), MaxRow(MaxRow), MinCol(MinCol), MaxCol(MaxCol),
    TableLength(TableLength), MapTileLength(MapTileLength), isValid(isValid)
{}

/* Пустой конструктор по умолчанию.
 * Созданный с помощью него экземпляр не будет считаться валидным.
 * Создавать такой объект следует, если функция должна вернуть
 * экземпляр MapDimensions, но её работа была выполнена неправильно */
FMapDimensions::FMapDimensions() : FMapDimensions(0, 0, 0, 0, TableLength, MapTileLength, false)
{}