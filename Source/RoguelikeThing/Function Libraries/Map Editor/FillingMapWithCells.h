// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <Components/UniformGridPanel.h>
#include "RoguelikeThing/Map/MapMatrix.h"
#include "RoguelikeThing/CoordWrapperOfTable.h"
#include "RoguelikeThing/MyGameInstance.h"
#include "RoguelikeThing/Enumerations/MapEditorBrushType.h"
#include <RoguelikeThing/Map/TileBuffer.h>
#include "FillingMapWithCells.generated.h"

/**********************************************************************************************************************
 * 
 * ������ ����� �������� ��������������� ��� ���������� ������ ����� ������������������������ ��������
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

DECLARE_LOG_CATEGORY_EXTERN(FillingMapWithCells, Log, All);

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UFillingMapWithCells : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	//������ ��������, �������� ����� ������������� �� ����� ���������� �����
	UPROPERTY()
	ULoadingWidget* LoadingWidget;

	//���������� ����� � ������� �����
	int ColsNum;
	//���������� ����� � ������ �����
	int RowsNum;

	int NumberOfTilesColsThatFitOnScreen = 0;
	int NumberOfTilesRowsThatFitOnScreen = 0;

	FVector2D TileSize = FVector2D(0, 0);
	
	UPROPERTY()
	UUniformGridSlot* MinTileGridSlot;

	//�������� �������� ������ ��� ���������� ���������� ����
	UPROPERTY()
	UMyGameInstance* GameInstance;
	
public:
	UFillingMapWithCells();
	/* �������, ����������� ����� �������� �� ������ ��.
	 *
	 * MapTileClass ����������� ������ ���� �����������
	 * ������ UMapTile ��� �� �����, CellClass �����������
	 * ������ ���� ����������� ������ UMapCell ��� �� ����� */
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "MapTileClass must necessarily be an heir of the UMapTile class or be UMapTile, CellClass must necessarily be an heir of the UMapCell class or be UMapCell"))
	void FillMapEditorWithCells(FMapDimensions MapDimensions, UUniformGridPanel* TilesGridPanel, UClass* CellClass, UClass* MapTileClass, UTileBuffer* TileBuf,
		UMapEditor* MapEditor, UCoordWrapperOfTable* TilesCoordWrapper, UMapMatrix* MapMatrix, FVector2D WidgetAreaSize);

	//���� �� �������� ������ ��������, �� �������� ����� ����� ��� ���������
	UFUNCTION(BlueprintCallable)
	void setLoadWidget(ULoadingWidget* newLoadingWidget);

	//������ ���������� ���������� �� �����������
	UFUNCTION(BlueprintCallable)
	int32 GetColNum();
	//������ ���������� ���������� �� ���������
	UFUNCTION(BlueprintCallable)
	int32 GetRowNum();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridCoord MinOriginalVisibleTile = FGridCoord();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridCoord MaxOriginalVisibleTile = FGridCoord();
};
