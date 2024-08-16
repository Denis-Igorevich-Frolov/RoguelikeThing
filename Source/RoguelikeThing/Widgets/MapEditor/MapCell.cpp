// Denis Igorevich Frolov did all this. Once there. All things reserved.


#include "RoguelikeThing/Widgets/MapEditor/MapCell.h"
#include <Kismet/GameplayStatics.h>

DEFINE_LOG_CATEGORY(MapCell);

UMapCell::UMapCell(const FObjectInitializer& Object) : UAbstractTile(Object)
{
    //��������� GameInstance �� ����
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

/* �������, ������������ ���������� ���������� ������, �� ���� � ������ ���������� ������������� �����. ���, ��������,
 * ������ 0:1, ����������� � ����� 0:3, ��� ����� ������� ����� � 5, ����� ����� ���������� ���������� 0:16 */
FCellCoord UMapCell::GetMyGlobalCoord()
{
    return FCellCoord(CoordOfParentTile->Row * MapTileLength + MyCoord.Row, CoordOfParentTile->Col * MapTileLength + MyCoord.Col);
}

//�������, ������������ ���������� ������ ������ ������ ������������� �����
FCellCoord UMapCell::GetMyLocalCoord()
{
    return MyCoord;
}
