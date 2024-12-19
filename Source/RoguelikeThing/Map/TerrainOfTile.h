// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <RoguelikeThing/Widgets/MapEditor/CellCoord.h>
#include "RoguelikeThing/Enumerations/CellType.h"
#include "TerrainOfTile.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(TerrainOfTile, Log, All);

/**********************************************************************************************************
 * Данный класс является классом предзагрузки структуры тайла. Хранит в себе все не нулевые ячейки тайла
 **********************************************************************************************************/

UCLASS(BlueprintType)
class ROGUELIKETHING_API UTerrainOfTile : public UObject
{
	GENERATED_BODY()

private:
	TMap<int, TMap<int, FCellType>> TerrainOfTileRows;

public:
	//Функция, добавляющая в переменную предзагрузки тайла тип не пустой ячейки
	UFUNCTION(BlueprintCallable)
	void AddCellType(FCellCoord Coord, FCellType CellType);

	//Функция, возвращающая тип ячейки по переданной координате. Если стиль не обнаружен, возвращается FCellType::Error
	UFUNCTION(BlueprintCallable)
	FCellType GetCellType(FCellCoord Coord);

	//Функция, проверяющая есть ли в переменной предзагрузки стиль по данной координате
	UFUNCTION(BlueprintCallable)
	bool Contains(FCellCoord Coord);

	//Получение массива всех не нулевых ячеек тайла
	UFUNCTION(BlueprintCallable)
	TArray<FCellCoord> GetFilledCoord();

	//Функция, удаляющая тип ячейки по переданной координате из переменной предзагрузки. Функция возвращает удачно ли была выполнена операция
	UFUNCTION(BlueprintCallable)
	bool RemoveCell(FCellCoord Coord);
};