// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <Components/UniformGridPanel.h>
#include "RoguelikeThing/Map/MapMatrix.h"
#include "RoguelikeThing/CoordWrapperOfTable.h"
#include "FillingMapWithCells.generated.h"

/***********************************************************
 * ������ ����� �������� ��������������� ��� ���������� 
 * ������ ����� ������������������������ ��������
 ***********************************************************/

DECLARE_LOG_CATEGORY_EXTERN(FillingMapWithCells, Log, All);

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UFillingMapWithCells : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	//������ ��������, �������� ����� ������������� �� ����� ���������� �����
	ULoadingWidget* LoadingWidget;

	//���������� ����� � ������� �����
	int ColNum;
	//���������� ����� � ������ �����
	int RowNum;
	
public:
	/* �������, ����������� ������� �������� �����.
	 *
	 * MapTileClass ����������� ������ ���� �����������
	 * ������ UMapTile ��� �� �����, CellClass �����������
	 * ������ ���� ����������� ������ UMapCell ��� �� ����� */
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "MapTileClass must necessarily be an heir of the Umatilla class or be itself, CellClass must necessarily be an heir of the UMapCell class or be itself"))
	bool FillMapEditorWithCells(FMapDimensions MapDimensions, UUniformGridPanel* TileGridPanel,
		UClass* CellClass, UClass* MapTileClass, UMapEditor* MapEditor, UCoordWrapperOfTable* TilesCoordWrapper);

	//���� �� �������� ������ ��������, �� �������� ����� ����� ��� ���������
	UFUNCTION(BlueprintCallable)
	void setLoadWidget(ULoadingWidget* newLoadingWidget);

	//������ ���������� ���������� �� �����������
	UFUNCTION(BlueprintCallable)
	int32 GetColNum();
	//������ ���������� ���������� �� ���������
	UFUNCTION(BlueprintCallable)
	int32 GetRowNum();
};
