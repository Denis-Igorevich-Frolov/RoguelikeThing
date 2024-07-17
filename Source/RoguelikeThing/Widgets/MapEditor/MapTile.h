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
 * Данный класс является базовым классом для виджета тайла. Он
 * нужен для правильного взаимодействия блюпринтов и c++ кода.
 *****************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(Map_Tile, Log, All);

UCLASS()
class ROGUELIKETHING_API UMapTile : public UAbstractTile
{
	GENERATED_BODY()

private:
	//Менеджер высокого уровня для экземпляра запущенной игры
	UPROPERTY()
	UMyGameInstance* GameInstance;

	UPROPERTY()
	TArray<UMapCell*> FilledCells;

public:
	UMapTile(const FObjectInitializer& Object);

	void SetMyCoord(FCellCoord myCoord) override;
	void ClearFilledCells() override;
	void OnAddedEvent(int MapTileLength, UMapMatrix* MapMatrix) override;

	//Координатная обёртка, хранящая в себе ячейки этого тайла
	UCoordWrapperOfTable* CellsCoordWrapper;

	UFUNCTION(BlueprintCallable)
	UCoordWrapperOfTable* GetCellsCoordWrapper();

	UFUNCTION(BlueprintImplementableEvent)
	UUniformGridPanel* GetGridPanel();

	UFUNCTION(BlueprintCallable)
	bool FillingWithCells(int MapTileLength, UClass* CellClass, UMapEditor* MapEditor, UMapMatrix* MapMatrix = nullptr);

	UFUNCTION(BlueprintCallable)
	void SetStyleFromDB(UMapCell* Cell, int row, int col, int MapTileLength, UMapMatrix* MapMatrix);

	UFUNCTION(BlueprintCallable)
	void SetStyleFromDBForAllCells(int MapTileLength, UMapMatrix* MapMatrix);
	
	UFUNCTION(BlueprintCallable)
	void AddFilledCell(UMapCell* Cell);
};
