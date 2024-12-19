// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RoguelikeThing/Map/TerrainOfTilesContainer.h"
#include "RoguelikeThing/Widgets/MapEditor/CellCoord.h"
#include "MySaveGame.generated.h"

/*******************************************************************
 * ������ ����� �������� �������, ��������� ���� ���������� .sav
 *******************************************************************/

UCLASS(BlueprintType)
class ROGUELIKETHING_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()

private:
	/* ���������, �������� ��� ���������� ������������ ������� �����. ����� ����������� ���������
	 * ������������ ����� ����� � �������� �� ����� .sav ���������� ������� ������� ReCreateTerrains */
	UPROPERTY(VisibleAnywhere)
	UTerrainOfTilesContainer* TerrainOfTilesContainer;

	//�������� ������ ��� ���������� � �������� ���������� TerrainOfTilesContainer
	UPROPERTY(VisibleAnywhere)
	TArray<uint8> BinTerrainOfTilesContainer;
	
public:

	//�������� ���������� �����
	UPROPERTY(VisibleAnywhere)
	FMapDimensions MapDimensions;

	/* ����������� � ������������ ���������� �������� ������ �����. ����� ������ �������
	 * ReCreateTerrains ���������� TerrainOfTilesContainer ������ ���������� ��������������� */
	UPROPERTY(VisibleAnywhere)
	FCellCoord MinNoEmptyTileCoord = FCellCoord(-1, -1);
	UPROPERTY(VisibleAnywhere)
	FCellCoord MaxNoEmptyTileCoord = FCellCoord(-1, -1);

	//��� ����� .db, �� �������� ������������ �������� ����������� �� ����� ���� ���� ������ � ������� ���������� ����������
	UPROPERTY(VisibleAnywhere)
	FString MapDataBaseHash;

	//�������, ������������� ���������� ���������� terrainOfTilesContainer
	void SaveTerrainOfTilesContainer(UTerrainOfTilesContainer* terrainOfTilesContainer);
	//�������, ��������������� ���������� ���������� terrainOfTilesContainer
	UTerrainOfTilesContainer* LoadTerrainOfTilesContainer();
};
