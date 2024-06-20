// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include <RoguelikeThing/MyGameInstance.h>
#include <RoguelikeThing/CoordWrapperOfTable.h>
#include <RoguelikeThing/Map/TileBuffer.h>
#include "TileTablesOptimizationTools.generated.h"

/**********************************************************************************************************************
 * 
 * Данный класс является библиотекой инструментов оптимизации тайловых таблиц.
 * 
 * Библиотека работает с таблицами UniformGridPanel, обёрнутыми в координатную
 * обёртку CoordWrapperOfTable и находящимися в Shift&ZoomArea. Всё содержимое
 * таблицы по идее должно состоять из виджетов-тайлов, но по факту код будет
 * работать с абсолютно любыми UWidget'ами, которые помещены в таблицу.
 * 
 * Задача оптимизатора скрывать те тайлы, что в данный момент не видны и отображать
 * те, что видны.
 * 
 * В метод оптимизации необходимо помещать тайловую таблицу, все элементы которой уже
 * схлопнуты (Collapsed). Перед началом работы метода оптимизации обязательно следует
 * вызвать функцию InitTableTiles для первичной инициализации.
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

DECLARE_LOG_CATEGORY_EXTERN(TileTablesOptimizationTools, Log, All);

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UTileTablesOptimizationTools : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	//Менеджер высокого уровня для экземпляра запущенной игры
	UMyGameInstance* GameInstance;

	UTileBuffer* TilesBuf;
	UCoordWrapperOfTable* TilesCoordWrapper;

	bool IsInit = false;
	
public:
	UTileTablesOptimizationTools();

	UFUNCTION(BlueprintCallable)
	void Init(UCoordWrapperOfTable* tilesCoordWrapper, UTileBuffer* tilesBuf, FGridDimensions originalDimensions);

	UFUNCTION(BlueprintCallable)
	void ChangeDisplayAreaFromShift(FVector2D TileShift);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridDimensions OldDimensions = FGridDimensions();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridDimensions CurrentDimensions = FGridDimensions();
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridDimensions OriginalDimensions = FGridDimensions();
};
