// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Widgets/MapEditor/MapTile.h"

UMapTile::UMapTile(const FObjectInitializer& Object) : UUserWidget(Object)
{
    CellsCoordWrapper = NewObject<UCoordWrapperOfTable>();
}
