// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <Components/UniformGridPanel.h>
#include "RoguelikeThing/Map/MapMatrix.h"
#include "RoguelikeThing/CoordWrapperOfTable.h"
#include "RoguelikeThing/MyGameInstance.h"
#include "RoguelikeThing/Enumerations/MapEditorBrushType.h"
#include "FillingMapWithCells.generated.h"

/**********************************************************************************************************************
 * 
 * Данный класс является инструментарием для заполнения пустой карты непроинициализированными ячейками
 * 
 **********************************************************************************************************************
 *
 *   Термины:
 *
 *** Карта (Контент, тайловая таблица) - глобальная корневая таблица для всех остальных составляющих частей. По
 *   идее это может быть любая тайловая таблица, но в данный момент это так или иначе только карта.
 *
 *** Фрагмент (Чанк) - таблица, из множества которых состоит карта. Предстваляет из себя матрицу в БД, заполненную
 *   состояниями ячеек. Каждый фрагмент хранит в себе кусочек карты по соответствующей координате. Также на
 *   фрагменты разбита и таблица виджета карты для лёгкой загрузки и сохранения в БД состояний ячеек из UI.
 *
 *** Тайл - таблица, из множества которых состоит фрагмент в виджете карты. В отличие от фрагмента, тайл участвует
 *   только в отрисовке UI и никак не отображается в БД. Он требуется сугубо для оптимизации - нужные тайлы видимы,
 *   ненужные - нет.
 *
 *** Ячейка - минимальная единица карты. Вся карта состоит из ячеек, у которых есть некоторые параметры.
 *
 **********************************************************************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(FillingMapWithCells, Log, All);

USTRUCT(BlueprintType)
struct FNumberOfTilesThatFit
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Cols = -1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Rows = -1;

	FNumberOfTilesThatFit(int Cols = -1, int Rows = -1);
};

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UFillingMapWithCells : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	//Виджет загрузки, корторый будет проигрываться во время заполнения карты
	ULoadingWidget* LoadingWidget;

	//Количество ячеек в столбце карты
	int ColsNum;
	//Количество ячеек в строке карты
	int RowsNum;

	int NumberOfMapTilesCols = 0;
	int NumberOfMapTilesRows = 0;

	FVector2D TileSize = FVector2D(0, 0);
    FVector2D ChunkSize = FVector2D(0, 0);

	//Менеджер высокого уровня для экземпляра запущенной игры
	UMyGameInstance* GameInstance;
	
public:
	UFillingMapWithCells();
	/* Функция, заполняющая карту ячейками на основе БД.
	 *
	 * MapTileClass обязательно должен быть наследником
	 * класса UMapTile или им самим, CellClass обязательно
	 * должен быть наследником класса UMapCell или им самим */
	UFUNCTION(BlueprintCallable, meta = (ToolTip = "MapTileClass must necessarily be an heir of the UMapTile class or be UMapTile, CellClass must necessarily be an heir of the UMapCell class or be UMapCell"))
	FNumberOfTilesThatFit FillMapEditorWithCells(FMapDimensions MapDimensions, UUniformGridPanel* TilesGridPanel, UClass* CellClass, UClass* MapTileClass,
		UMapEditor* MapEditor, UCoordWrapperOfTable* TilesCoordWrapper, UMapMatrix* MapMatrix, FVector2D WidgetAreaSize, float MaxDiffSizeFromScalingToLargerSide);

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
