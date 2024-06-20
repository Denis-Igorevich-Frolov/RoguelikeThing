// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <RoguelikeThing/MyGameInstance.h>
#include <RoguelikeThing/CoordWrapperOfTable.h>
#include <RoguelikeThing/Map/TileBuffer.h>
#include "TileTablesOptimizationTools.generated.h"

/**********************************************************************************************************************
 * 
 * ������ ����� �������� ����������� ������������ ����������� �������� ������.
 * 
 * ���������� �������� � ��������� UniformGridPanel, ��������� � ������������
 * ������ CoordWrapperOfTable � ������������ � Shift&ZoomArea. �� ����������
 * ������� �� ���� ������ �������� �� ��������-������, �� �� ����� ��� �����
 * �������� � ��������� ������ UWidget'���, ������� �������� � �������.
 * 
 * ������ ������������ �������� �� �����, ��� � ������ ������ �� ����� � ����������
 * ��, ��� �����.
 * 
 * � ����� ����������� ���������� �������� �������� �������, ��� �������� ������� ���
 * ��������� (Collapsed). ����� ������� ������ ������ ����������� ����������� �������
 * ������� ������� InitTableTiles ��� ��������� �������������.
 * 
 **********************************************************************************************************************
 *
 *   �������:
 *
 *** ����� (�������, �������� �������) - ���������� �������� ������� ��� ���� ��������� ������������ ������. ��
 *   ���� ��� ����� ���� ����� �������� �������, �� � ������ ������ ��� ��� ��� ����� ������ �����.
 *
 *** �������� (����) - �������, �� ��������� ������� ������� �����. ������������ �� ���� ������� � ��, �����������
 *   ����������� �����. ������ �������� ������ � ���� ������� ����� �� ��������������� ����������. ����� ��
 *   ��������� ������� � ������� ������� ����� ��� ����� �������� � ���������� � �� ��������� ����� �� UI.
 *
 *** ���� - �������, �� ��������� ������� ������� �������� � ������� �����. � ������� �� ���������, ���� ���������
 *   ������ � ��������� UI � ����� �� ������������ � ��. �� ��������� ������ ��� ����������� - ������ ����� ������,
 *   �������� - ���.
 *
 *** ������ - ����������� ������� �����. ��� ����� ������� �� �����, � ������� ���� ��������� ���������.
 *
 **********************************************************************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(TileTablesOptimizationTools, Log, All);

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UTileTablesOptimizationTools : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	//�������� �������� ������ ��� ���������� ���������� ����
	UMyGameInstance* GameInstance;

	UTileBuffer* TilesBuf;
	UCoordWrapperOfTable* TilesCoordWrapper;

	bool IsInit = false;
	
public:
	UTileTablesOptimizationTools();

	UFUNCTION(BlueprintCallable)
	void Init(UCoordWrapperOfTable* tilesCoordWrapper, UTileBuffer* tilesBuf, FGridDimensions originalDimensions);

	UFUNCTION(BlueprintCallable)
	void ChangeDisplayAreaFromShift(FVector2D TileShift);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridDimensions OldDimensions = FGridDimensions();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridDimensions CurrentDimensions = FGridDimensions();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridDimensions OriginalDimensions = FGridDimensions();
};
