// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/UserWidget.h"
#include <Components/UniformGridSlot.h>
#include "RoguelikeThing/Widgets/Abstract/AbstractTile.h"
#include "CoordWrapperOfTable.generated.h"

/*******************************************************************************************************************************************
 * ������ ����� �������� ������������ ������� ��� UniformGridPanel. � ������� ���� ������ ����� ����� �������� ������� �� ����������
 * ������ � �������. ����� ������� ����������� ������ �������� ������ (����� ������) � ��������� (������ �������) �������. ��� ������
 * � ������� ����� ����� ������������� ���������� �����, ��� ��������� ��������� ������� � ����� ��������.
 *******************************************************************************************************************************************/

/* ��� ��� � ������� ������� ������ �������� ������ ��� ������, ����� �������� ���� ����� - ��������,
 * ������� � ���� ������, �������������� �� ���� ������ ��������� �������. ����� �� ���� ����� �����
 * ������ ������ ������, ������� �� ���� ��� ����� ������������ ������� ����� �������� ������� */
class WrapperRow {
private:
	TMap<int32, UUniformGridSlot*> RowGrid;
public:
	TMap<int32, UAbstractTile*> Row;
	~WrapperRow();
	UAbstractTile* FindWidget(int Key);
	UUniformGridSlot* FindGridSlot(int Key);
	bool RemoveWidget(int Key);
	bool RemoveIndex(int Key);
	bool AddWidget(int Key, UAbstractTile* Widget, UUniformGridSlot* GridSlot);
	bool HasAnyEllements();
	bool Contains(int Key);
};

/* ��� ��������� ���������� ����������. ���� ������, ���� ������� - �� ������. ���� ���������� ���� 
 * ������� ����� ������ ������������ ��� ���������������������, ����� ��� ��������� ������ � ������ */
USTRUCT(BlueprintType)
struct FGridCoord
{
	GENERATED_BODY()

private:
	bool isInit = false;

public:
	/* �������� ���������� ����� ������ ����������� ������ ���������� �����������������������, ����� ������
	 * �� ����� �������� � ����� �����������. ��������� ���������������������� ���������� ��� �������, �������
	 * � ����� ������ ���� ������� �����-������ ����������, � ������� ���������������������� ��������� �� �����
	 * ������� �������� ������ ��� �� ���������� */
	FGridCoord();
	FGridCoord(int row, int col);
	//�������� ����������������������� ����������
	bool getIsInit();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Col;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Row;

	FString ToString();

	bool operator == (const FGridCoord& Coord) const;
	bool operator != (const FGridCoord& Coord) const;

	FGridCoord operator + (const FGridCoord Bias) const;
	FGridCoord operator - (const FGridCoord Bias) const;
};

USTRUCT(BlueprintType)
struct FGridDimensions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridCoord Min;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridCoord Max;

	FGridDimensions(FGridCoord min = FGridCoord(), FGridCoord max = FGridCoord());

	FString ToString();
	bool IsEmpty();
	bool DoTheDimensionsIntersect(FGridDimensions OtherDimensions);

	bool operator == (const FGridDimensions& Dimensions) const;
	bool operator != (const FGridDimensions& Dimensions) const;
	FGridDimensions operator + (const FGridDimensions Bias) const;
	FGridDimensions operator - (const FGridDimensions Bias) const;
};

UCLASS(BlueprintType)
class ROGUELIKETHING_API UCoordWrapperOfTable : public UObject
{
	GENERATED_BODY()
private:
	TMap<int32, WrapperRow*> Col;
	int NumberOfItemsInTable = 0;

	FGridCoord MinCoord;
	FGridCoord MaxCoord;

public:
	~UCoordWrapperOfTable();

	UFUNCTION(BlueprintCallable)
	bool AddWidget(int row, int col, UAbstractTile* Widget, UUniformGridSlot* GridSlot);
	UFUNCTION(BlueprintCallable)
	UAbstractTile* FindWidget(int row, int col);
	UFUNCTION(BlueprintCallable)
	UUniformGridSlot* FindGridSlot(int row, int col);
	UFUNCTION(BlueprintCallable)
	bool RemoveCoord(int row, int col);
	UFUNCTION(BlueprintCallable)
	bool RemoveWidget(int row, int col);
	UFUNCTION(BlueprintCallable)
	bool HasAnyEllements();
	UFUNCTION(BlueprintCallable)
	const int getNumberOfItemsInTable();
	UFUNCTION(BlueprintCallable)
	void Clear();

	//��������� ����� ����� ������ ����������
	UFUNCTION(BlueprintCallable)
	FGridCoord getMinCoord();
	//��������� ����� ������ ������� ����������
	UFUNCTION(BlueprintCallable)
	FGridCoord getMaxCoord();

	UFUNCTION(BlueprintCallable)
	bool DoTheDimensionsIntersect(FGridDimensions Dimensions_1, FGridDimensions Dimensions_2);

	void setMinCoord(FGridCoord minCoord);
	void setMaxCoord(FGridCoord maxCoord);
};
