// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RoguelikeThing/Map/MapMatrix.h"
#include "RoguelikeThing/CoordWrapperOfTable.h"
#include "TileTablesOptimizationTools.generated.h"

/*****************************************************************************************
 * ������ ����� �������� ����������� ������������ ����������� �������� ������.
 * 
 * ���������� �������� � ��������� UniformGridPanel, ��������� � ������������
 * ������ CoordWrapperOfTable � ������������ � Shift&ZoomArea. �� ����������
 * ������� �� ���� ������ �������� �� ��������-������, �� �� ����� ��� �����
 * �������� � ��������� ������ UWidget'���, ������� �������� � �������.
 * 
 * ������ ������������ �������� �� �����, ��� � ������ ������ �� ����� � ����������
 * ��, ��� �����.
 *****************************************************************************************/

//��������� ���������� �����
USTRUCT(BlueprintType)
struct FTileCoord
{
    GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 X;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Y;

	FTileCoord(int32 x = 0, int32 y = 0);

	//���������������� ���������� � ����� ��� ������ � ����
	FString ToString();

	bool operator == (const FTileCoord& Coord) const;
	bool operator != (const FTileCoord& Coord) const;

	FTileCoord operator + (const FTileCoord Bias) const;
	FTileCoord operator - (const FTileCoord Bias) const;
};

//��������� ��������� ������� �������, ������� �������� � ���� ����������� � ������������ ���������� ������ � ���
USTRUCT(BlueprintType)
struct FDimensionsDisplayedArea
{
	GENERATED_BODY()

	//����� ����� ������ ����������
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTileCoord Min;

	//����� ������ ������� ����������
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTileCoord Max;

	FDimensionsDisplayedArea(FTileCoord min = FTileCoord(), FTileCoord max = FTileCoord());

	//���������������� ��������� � ����� ��� ������ � ����
	FString ToString();
	bool IsEmpty();

	bool operator == (const FDimensionsDisplayedArea& Dimensions) const;
	bool operator != (const FDimensionsDisplayedArea& Dimensions) const;
	FDimensionsDisplayedArea operator + (const FDimensionsDisplayedArea Bias) const;
	FDimensionsDisplayedArea operator - (const FDimensionsDisplayedArea Bias) const;
};

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UTileTablesOptimizationTools : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	int TileLen;
	int TableRows;
	int TableCols;
	int RealTableRows;
	int RealTableCols;

	//������ ������� ��� ����� ���������������
	FVector2D OriginalTableSize;
	//������ ����� ��� ����� ���������������
	FVector2D OriginalTileSize;
	//������ ������� �������, ������ �������� �������� ������� � �������
	FVector2D widgetAreaSize;
	/* ��������� ������ ������, ������� ���������� ������� � �������
	 * �������, ������ �������� �������� ������� � ������� */
	FVector2D OriginalDimensionsSize;
	// ������� ����������� �������� ������� � �������, � ������� ��� ��������
	FVector2D SizeDifference;
	//����������� ������ ������� � �������. ���� �������� ������ ���� ������������, ���������� �������
	FVector2D minContentSize;
	//���������� ��� ����������� �������� ���������� ����� ����
	FVector2D DistanceToAppearanceOfNewTiles;

	//���������� �������� ������������ ������
	FDimensionsDisplayedArea OldDimensions = FDimensionsDisplayedArea();
	//������� �������� ������������ ������
	FDimensionsDisplayedArea CurrentDimensions = FDimensionsDisplayedArea();
	//�������� �������, ������� ������������ ����������
	FDimensionsDisplayedArea OriginalDimensions = FDimensionsDisplayedArea();
	
public:
	/* ��������� ������������� ������� ������ ����� �������, ����� � ��� ��������
	 * ���� ����� �� �����, ������� ������ � ������� ������������� ������� */
	UFUNCTION(BlueprintCallable)
    FVector2D InitTableTiles(UCoordWrapperOfTable* TilesCoordWrapper, FVector2D TileSize,
		FVector2D WidgetAreaSize, FMapDimensions MapDimensions, FVector2D MinContentSize = FVector2D(2000, 2000));

	/* �������, ���������� ������������ � ������ ������ �����, ����� ��� ������� ���������
	 * ����������. ������������ ��� ������ ��������� ������� ����� ������������������ */
	UFUNCTION(BlueprintCallable)
	void ExtinguishCurrentDimension(UCoordWrapperOfTable* TilesCoordWrapper);

	//�������, ���������� ��������� ������ �� ������ ��� ��������������� �������
	UFUNCTION(BlueprintCallable)
	void ChangingVisibilityOfTableTiles(UCoordWrapperOfTable* TilesCoordWrapper, FVector2D Bias, float ZoomMultiplier);
};
