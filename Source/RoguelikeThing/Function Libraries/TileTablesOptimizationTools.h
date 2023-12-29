// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RoguelikeThing/Map/MapMatrix.h"
#include "RoguelikeThing/CoordWrapperOfTable.h"
#include "TileTablesOptimizationTools.generated.h"

/*****************************************************************************************
 * Данный класс является библиотекой инструментов оптимизации тайловых таблиц.
 * 
 * Библиотека работает с таблицами UniformGridPanel, обёрнутыми в координатную
 * обёртку CoordWrapperOfTable и находящимися в Shift&ZoomArea. Всё содержимое
 * таблицы по идее должно состоять из виджетов-тайлов, но по факту код будет
 * работать с абсолютно любыми UWidget'ами, которые помещены в таблицу.
 * 
 * Задача оптимизатора скрывать те тайлы, что в данный момент не видны и отображать
 * те, что видны.
 *****************************************************************************************/

//Структура координаты тайла
USTRUCT(BlueprintType)
struct FTileCoord
{
    GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 X;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Y;

	FTileCoord(int32 x = 0, int32 y = 0);

	//Преобразовывание координаты в текст для вывода в логи
	FString ToString();

	bool operator == (const FTileCoord& Coord) const;
	bool operator != (const FTileCoord& Coord) const;

	FTileCoord operator + (const FTileCoord Bias) const;
	FTileCoord operator - (const FTileCoord Bias) const;
};

//Структура габаритов области таблицы, которая включает в себя минимальную и максимальную координаты тайлов в ней
USTRUCT(BlueprintType)
struct FDimensionsDisplayedArea
{
	GENERATED_BODY()

	//Самая левая нижняя координата
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTileCoord Min;

	//Самая правая верхняя координата
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTileCoord Max;

	FDimensionsDisplayedArea(FTileCoord min = FTileCoord(), FTileCoord max = FTileCoord());

	//Преобразовывание габаритов в текст для вывода в логи
	FString ToString();
	bool IsEmpty();

	bool operator == (const FDimensionsDisplayedArea& Dimensions) const;
	bool operator != (const FDimensionsDisplayedArea& Dimensions) const;
	FDimensionsDisplayedArea operator + (const FDimensionsDisplayedArea Bias) const;
	FDimensionsDisplayedArea operator - (const FDimensionsDisplayedArea Bias) const;
};

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UTileTablesOptimizationTools : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

private:
	int TileLen;
	int TableRows;
	int TableCols;
	int RealTableRows;
	int RealTableCols;

	//Ращмер таблицы без учёта масштабирования
	FVector2D OriginalTableSize;
	//Ращмер тайла без учёта масштабирования
	FVector2D OriginalTileSize;
	//Размер области виджета, внутрь которого помещена таблица с тайлами
	FVector2D widgetAreaSize;
	/* Суммарный размер тайлов, которые изначально влезают в область
	 * виджета, внутрь которого помещена таблица с тайлами */
	FVector2D OriginalDimensionsSize;
	// Разница изначальных размеров таблицы и виджета, в который она помещена
	FVector2D SizeDifference;
	//Минимальный размер таблицы с тайлами. Если реальный размер ниже минимального, появляются отступы
	FVector2D minContentSize;
	//Расстояние при преодолении которого появляется новый тайл
	FVector2D DistanceToAppearanceOfNewTiles;

	//Предыдущие габариты отображаемых тайлов
	FDimensionsDisplayedArea OldDimensions = FDimensionsDisplayedArea();
	//Текущие габариты отображаемых тайлов
	FDimensionsDisplayedArea CurrentDimensions = FDimensionsDisplayedArea();
	//Габариты области, которая отображалась изначально
	FDimensionsDisplayedArea OriginalDimensions = FDimensionsDisplayedArea();
	
public:
	/* Первичная инициализация таблицы тайлов таким образом, чтобы в ней видимыми
	 * были тольо те тайлы, которые влезли в область родительского виджета */
	UFUNCTION(BlueprintCallable)
    FVector2D InitTableTiles(UCoordWrapperOfTable* TilesCoordWrapper, FVector2D TileSize,
		FVector2D WidgetAreaSize, FMapDimensions MapDimensions, FVector2D MinContentSize = FVector2D(2000, 2000));

	/* Функция, скрывающая отображаемые в данный момент тайлы, делая всю таблицу полностью
	 * неактивной. Используется для сброса состояния таблицы перед переинициализацией */
	UFUNCTION(BlueprintCallable)
	void ExtinguishCurrentDimension(UCoordWrapperOfTable* TilesCoordWrapper);

	//Функция, изменяющая видимость тайлов от сдвига или масштабирования таблицы
	UFUNCTION(BlueprintCallable)
	void ChangingVisibilityOfTableTiles(UCoordWrapperOfTable* TilesCoordWrapper, FVector2D Bias, float ZoomMultiplier);
};
