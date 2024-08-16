// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RoguelikeThing/Widgets/Abstract/AbstractTile.h"
#include <RoguelikeThing/CoordWrapperOfTable.h>
#include <Components/UniformGridPanel.h>
#include <RoguelikeThing/MyGameInstance.h>
#include <RoguelikeThing/Map/MapMatrix.h>
#include "MapCell.h"
#include "MapTile.generated.h"

/*****************************************************************
 * ������ ����� �������� ������� ������� ��� ������� �����
 *****************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(MapTile, Log, All);

UCLASS()
class ROGUELIKETHING_API UMapTile : public UAbstractTile
{
	GENERATED_BODY()

private:
	//�������� �������� ������ ��� ���������� ���������� ����
	UPROPERTY()
	UMyGameInstance* GameInstance;

	//������ ����� ����� ��� ����� ���������� �� ��������
	UPROPERTY()
	TArray<UMapCell*> FilledCells;

	//��������� ������������, �������� ������ ��������� ����� ��� ����� ���������� �� ��������
	UPROPERTY()
	UTerrainOfTile* MyTerrainOfTile;

public:
	UMapTile(const FObjectInitializer& Object);
	~UMapTile();

	/* ��������������� ����������� ������� ��� �������� ���� �����. ������� ������������ ��� ������� �����
	 * ���������. ��� ���������� ���������� ������������ ������ CellsCoordWrapper, ��� ��� ���� ����������
	 * �������� ����� ��� ��� ������������ ��������, CellsCoordWrapper ������� ����� ������������������� */
	void RemoveAllCells() override;
	void SetMyCoord(FCellCoord myCoord) override;
	/* ��������������� ����������� ������� ��� ������ ��������� ��������� �����. ��� ������
	 * ������� ���������� MyTerrainOfTile ������������ � ������� ����������� ����������������� */
	void ClearFilledCells() override;
	//��������������� ����������� �������, ���������� ��� ���������� ����� � �������
	void OnAddedEvent(UMapMatrix* MapMatrix) override;

	//������������ ������, �������� � ���� ������ ����� �����
	UPROPERTY()
	UCoordWrapperOfTable* CellsCoordWrapper;

	void SetMyTerrainOfTile(UTerrainOfTile* TerrainOfTile);

	UFUNCTION(BlueprintCallable)
	UCoordWrapperOfTable* GetCellsCoordWrapper();

	//�������, ������������ ������� �����
	UFUNCTION(BlueprintImplementableEvent)
	UUniformGridPanel* GetGridPanel();

	//�������, ����������� ���� ��������. ���� MapMatrix ����� �������, �� � ����� ����� ����������� ��� ����������� �����, ����� ��� ������ ����� �� ������ �� ���������
	UFUNCTION(BlueprintCallable)
	bool FillingWithCells(int MapTileLength, UClass* CellClass, UMapEditor* MapEditor, UMapMatrix* MapMatrix = nullptr);

	//�������, �������� ����� ������ �� ���������� ���������� ������ �� ���������� ������������ MyTerrainOfTile
	UFUNCTION(BlueprintCallable)
	void SetStyleFromTerrainOfTile(UMapCell* Cell, int row, int col, int MapTileLength, UMapMatrix* MapMatrix);
	
	//�������, ����������� ���������� � �������, ����� ������� ���������� �� ��������
	UFUNCTION(BlueprintCallable)
	void UpdateInformationAboutCells(UMapCell* Cell, FMapEditorBrushType CellStyle);
	
	//�������, ���������� ����� ����� � ������������ � ���������� ������������ MyTerrainOfTile
	UFUNCTION(BlueprintCallable)
	bool FillCellsAccordingToTerrain(UMapMatrix* MapMatrix);
};
