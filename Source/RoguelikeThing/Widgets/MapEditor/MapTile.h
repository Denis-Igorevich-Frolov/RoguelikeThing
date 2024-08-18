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
 * Данный класс является базовым классом для виджета тайла
 *****************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(MapTile, Log, All);

UCLASS()
class ROGUELIKETHING_API UMapTile : public UAbstractTile
{
	GENERATED_BODY()

private:
	//Менеджер высокого уровня для экземпляра запущенной игры
	UPROPERTY()
	UMyGameInstance* GameInstance;

	//Массив ячеек тайла чей стиль отличается от базового
	UPROPERTY()
	TArray<UMapCell*> FilledCells;

	//Пременная предзагрузки, хранящая список координат ячеек чей стиль отличается от базового
	UPROPERTY()
	UTerrainOfTile* MyTerrainOfTile;

public:
	UMapTile(const FObjectInitializer& Object);
	~UMapTile();

	/* Переопределение виртуальной функции для удаления всех ячеек. Функция используется при очистке перед
	 * удалением. Она уничтожает переменную координатной обёртки CellsCoordWrapper, так что если необходимо
	 * очистить класс без его последующего удаления, CellsCoordWrapper следует вновь проинициализировать */
	void RemoveAllCells() override;
	void SetMyCoord(FCellCoord myCoord) override;
	/* Переопределение виртуальной функции для сброса состояния изменённых ячеек. При вызове
	 * функции переменная MyTerrainOfTile уничтожается и требует последующей переинициализации */
	void ClearFilledCells() override;
	//Переопределение виртуальной функции, вызываемой при добавлении тайла в таблицу
	void OnAddedEvent(UMapMatrix* Map) override;

	//Координатная обёртка, хранящая в себе ячейки этого тайла
	UPROPERTY()
	UCoordWrapperOfTable* CellsCoordWrapper;

	void SetMyTerrainOfTile(UTerrainOfTile* Terrain);

	UFUNCTION(BlueprintCallable)
	UCoordWrapperOfTable* GetCellsCoordWrapper();

	//Функция, возвращающая таблицу ячеек
	UFUNCTION(BlueprintImplementableEvent)
	UUniformGridPanel* GetGridPanel();

	//Функция, заполняющая тайл ячейками. Если MapMatrix будет передан, то у ячеек сразу подгрузятся все необходимые стили, иначе все ячейки будут со стилем по умолчанию
	UFUNCTION(BlueprintCallable)
	bool FillingWithCells(int MapTileLength, UClass* CellClass, UMapEditor* MapEditor, UMapMatrix* Map = nullptr);

	//Функция, задающая стиль ячееки по переданной координате исходя из переменной предзагрузки MyTerrainOfTile
	UFUNCTION(BlueprintCallable)
	void SetStyleFromTerrainOfTile(UMapCell* Cell, int row, int col, int MapTileLength, UMapMatrix* Map);
	
	//Функция, обновляющая информацию о ячейках, стиль которых отличается от базового
	UFUNCTION(BlueprintCallable)
	void UpdateInformationAboutCells(UMapCell* Cell, FMapEditorBrushType CellStyle);
	
	//Функция, преводящая стиль ячеек в соответствие с переменной предзагрузки MyTerrainOfTile
	UFUNCTION(BlueprintCallable)
	bool FillCellsAccordingToTerrain(UMapMatrix* Map);
};
