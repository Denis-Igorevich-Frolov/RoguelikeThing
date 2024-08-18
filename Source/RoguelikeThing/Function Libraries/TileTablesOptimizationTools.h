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
 * � ����� ����������� ���������� �������� �������� �������, � ������� ��� ����������
 * ������ �� �����, ������� ������� �� �����, � ������������ ����� �������� ��� �������
 * �����������. ����� ������� ������ ������ ����������� ����������� ������� �������
 * ������� Init ��� ��������� �������������.
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
	UPROPERTY()
	UMyGameInstance* GameInstance;

	UPROPERTY()
	UUniformGridPanel* TilesGridPanel;
	UPROPERTY()
	UCoordWrapperOfTable* TilesCoordWrapper;
	//����� �� ���� ������� �����
	UPROPERTY()
	UTileBuffer* TilesBuf;
	UPROPERTY()
	UMapMatrix* MapMatrix;

	//��������� ������ ���� ������ ����������� ��������
	FVector2D MaximumContentSize = FVector2D(0, 0);
	//������� � �������� ����� �������� ������������� ������� Shift&ZoomArea � ������������ ��������
	FVector2D SizeDifference = FVector2D(0, 0);
	//����������, ��� ����������� �������� ���������� ������ ����� ����
	FVector2D DistanceToAppearanceOfFirstNewTile = FVector2D(0, 0);
	//����������� ������ ������� � ������������ �������
	FVector2D OriginalDimensionsSize = FVector2D(0, 0);
	//������ �������� � ��������, ������� ��������� ����������� ������� ���������� ���� ������� ������ ��� ����������� �������, ���� �� ������ ��������
	FVector2D MinContentSize = FVector2D(0, 0);
	//������ �����
	FVector2D TileSize = FVector2D(0, 0);
	//������ ������������� ������� Shift&ZoomArea
	FVector2D WidgetAreaSize = FVector2D(0, 0);
	//���������� ������ � ������ � ������� ���� �����
	int NumberOfTileRowsInTable = 0;
	int NumberOfTileColsInTable = 0;

	bool IsInit = false;

	//������� ������������ ���������� ����� ������
	void AsynchronousAreaFilling(FGridDimensions AreaDimensions, int NumberOfMapTilesRows);
	//������� ������������ �������� ������ ������
	void AsynchronousAreaRemoving(FGridDimensions AreaDimensions, int NumberOfMapTilesRows);
	
public:
	UTileTablesOptimizationTools();

	UFUNCTION(BlueprintCallable)
	void Init(UUniformGridPanel* refTilesGridPanel, UCoordWrapperOfTable* refTilesCoordWrapper, UTileBuffer* refTilesBuf, UMapMatrix* refMapMatrix, FGridDimensions originalDimensions,
		FVector2D widgetAreaSize, FVector2D tileSize, FVector2D MinContentSize, int numberOfTileRowsInTable, int numberOfTileColsInTable);

	UFUNCTION(BlueprintCallable)
	//�������, ���������� ���������� ������������ ������ �� ������ ��� ��������������� �������
	void ChangingVisibilityOfTableTiles(FVector2D Bias, float ZoomMultiplier, int NumberOfMapTilesRows);

	//�������� �����, ������� ���� ��� ���������� ���������
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridDimensions OldDimensions = FGridDimensions();
	//������� �������� �����
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridDimensions CurrentDimensions = FGridDimensions();
	//�������� �����, ������� ���� ���������� ��� �������������
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridDimensions OriginalDimensions = FGridDimensions();
};
