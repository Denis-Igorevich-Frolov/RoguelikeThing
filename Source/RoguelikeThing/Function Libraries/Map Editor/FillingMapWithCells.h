// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <Components/UniformGridPanel.h>
#include "RoguelikeThing/Map/MapMatrix.h"
#include "RoguelikeThing/CoordWrapperOfTable.h"
#include "RoguelikeThing/MyGameInstance.h"
#include "FillingMapWithCells.generated.h"

/***********************************************************
 * Данный класс является инструментарием для заполнения 
 * пустой карты непроинициализированными ячейками
 ***********************************************************/

DECLARE_LOG_CATEGORY_EXTERN(FillingMapWithCells, Log, All);

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UFillingMapWithCells : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	//Виджет загрузки, корторый будет проигрываться во время заполнения карты
	ULoadingWidget* LoadingWidget;

	//Количество ячеек в столбце карты
	int ColNum;
	//Количество ячеек в строке карты
	int RowNum;

	//Менеджер высокого уровня для экземпляра запущенной игры
	UMyGameInstance* GameInstance;
	
public:
	UFillingMapWithCells();
	/* Функция, заполняющая пустыми ячейками карту.
	 *
	 * MapTileClass обязательно должен быть наследником
	 * класса UMapTile или им самим, CellClass обязательно
	 * должен быть наследником класса UMapCell или им самим */
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "MapTileClass must necessarily be an heir of the UMapTile class or be UMapTile, CellClass must necessarily be an heir of the UMapCell class or be UMapCell"))
	bool FillMapEditorWithCells(FMapDimensions MapDimensions, UUniformGridPanel* TilesGridPanel,
		UClass* CellClass, UClass* MapTileClass, UMapEditor* MapEditor, UCoordWrapperOfTable* TilesCoordWrapper);

	//Если не передать виджет загрузки, то загрузка будет будет без индикации
	UFUNCTION(BlueprintCallable)
	void setLoadWidget(ULoadingWidget* newLoadingWidget);

	//Геттер количества фрагментов по горизонтали
	UFUNCTION(BlueprintCallable)
	int32 GetColNum();
	//Геттер количества фрагментов по вертикали
	UFUNCTION(BlueprintCallable)
	int32 GetRowNum();
};
