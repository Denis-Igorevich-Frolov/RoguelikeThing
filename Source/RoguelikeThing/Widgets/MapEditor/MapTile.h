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
 * ƒанный класс €вл€етс€ базовым классом дл€ виджета тайла
 *****************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(MapTile, Log, All);

UCLASS()
class ROGUELIKETHING_API UMapTile : public UAbstractTile
{
	GENERATED_BODY()

private:
	//ћенеджер высокого уровн€ дл€ экземпл€ра запущенной игры
	UPROPERTY()
	UMyGameInstance* GameInstance;

	//ћассив €чеек тайла чей стиль отличаетс€ от базового
	UPROPERTY()
	TArray<UMapCell*> FilledCells;

	//ѕременна€ предзагрузки, хран€ща€ список координат €чеек чей стиль отличаетс€ от базового
	UPROPERTY()
	UTerrainOfTile* MyTerrainOfTile;

public:
	UMapTile(const FObjectInitializer& Object);
	~UMapTile();

	/* ѕереопределение виртуальной функции дл€ удалени€ всех €чеек. ‘ункци€ используетс€ при очистке перед
	 * удалением. ќна уничтожает переменную координатной обЄртки CellsCoordWrapper, так что если необходимо
	 * очистить класс без его последующего удалени€, CellsCoordWrapper следует вновь проинициализировать */
	void RemoveAllCells() override;
	void SetMyCoord(FCellCoord myCoord) override;
	/* ѕереопределение виртуальной функции дл€ сброса состо€ни€ изменЄнных €чеек. ѕри вызове
	 * функции переменна€ MyTerrainOfTile уничтожаетс€ и требует последующей переинициализации */
	void ClearFilledCells() override;
	//ѕереопределение виртуальной функции, вызываемой при добавлении тайла в таблицу
	void OnAddedEvent(UMapMatrix* MapMatrix) override;

	// оординатна€ обЄртка, хран€ща€ в себе €чейки этого тайла
	UPROPERTY()
	UCoordWrapperOfTable* CellsCoordWrapper;

	void SetMyTerrainOfTile(UTerrainOfTile* TerrainOfTile);

	UFUNCTION(BlueprintCallable)
	UCoordWrapperOfTable* GetCellsCoordWrapper();

	//‘ункци€, возвращающа€ таблицу €чеек
	UFUNCTION(BlueprintImplementableEvent)
	UUniformGridPanel* GetGridPanel();

	//‘ункци€, заполн€юща€ тайл €чейками. ≈сли MapMatrix будет передан, то у €чеек сразу подгруз€тс€ все необходимые стили, иначе все €чейки будут со стилем по умолчанию
	UFUNCTION(BlueprintCallable)
	bool FillingWithCells(int MapTileLength, UClass* CellClass, UMapEditor* MapEditor, UMapMatrix* MapMatrix = nullptr);

	//‘ункци€, задающа€ стиль €чееки по переданной координате исход€ из переменной предзагрузки MyTerrainOfTile
	UFUNCTION(BlueprintCallable)
	void SetStyleFromTerrainOfTile(UMapCell* Cell, int row, int col, int MapTileLength, UMapMatrix* MapMatrix);
	
	//‘ункци€, обновл€юща€ информацию о €чейках, стиль которых отличаетс€ от базового
	UFUNCTION(BlueprintCallable)
	void UpdateInformationAboutCells(UMapCell* Cell, FMapEditorBrushType CellStyle);
	
	//‘ункци€, превод€ща€ стиль €чеек в соответствие с переменной предзагрузки MyTerrainOfTile
	UFUNCTION(BlueprintCallable)
	bool FillCellsAccordingToTerrain(UMapMatrix* MapMatrix);
};
