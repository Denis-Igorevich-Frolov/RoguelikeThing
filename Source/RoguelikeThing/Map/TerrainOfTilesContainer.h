// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RoguelikeThing/Map/TerrainOfTile.h"
#include "RoguelikeThing/Enumerations/CellType.h"
#include "MapDimensions.h"
#include "TerrainOfTilesContainer.generated.h"

/*******************************************************************************************
 * Данный класс является контейнером для матрицы переменных предзагрузки UTerrainOfTile.
 * Также класс имеет возможность восстановить состояние этой матрыцы после загрузки
 * сохранения при помощи функции ReCreateTerrains
 *******************************************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(TerrainOfTilesContainer, Log, All);

//Класс строк матрицы переменных предзагрузки. Предназначен для обхода ограничения на вложенные массивы
UCLASS()
class ROGUELIKETHING_API UTerrainOfTilesRow : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<int, UTerrainOfTile*> TerrainOfTilesRow;
};

UCLASS()
class ROGUELIKETHING_API UTerrainOfTilesContainer : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<int, UTerrainOfTilesRow*> TerrainOfTilesRows;

	/* Контейнер с координатами и стилями всех ячеек карты. Является сериализуемым хранилищем
	 * без указателей для восстановления матрицы переменных предзагрузки после загрузки сохранения.
	 * 
	 * Для хранения координат используется FVector2D, а не превычный FCellCoord так как структура не
	 * может просто быть ключом TMap, ей необходимо правильным образом переопределить некоторые функции,
	 * отвечающие за уникальный идентификатор или что-то в этом роде. Короче, после нескольких неудачных
	 * попыток и компиляций длинною в пару вечностей, я не стал запариваться с этим, оно того явно не стоило.
	 * FVector2D уже всё это реализует сразу, а никакие функции из FCellCoord мне здесь всё равно не нужны
	 * кроме, непосредственно, хранения 2 чисел.
	 * Просто учтите, значение X и Y у ключа коллекции и при записи, и при считывании усекаются до int.
	 * X - это Row, Y - это Col */
	UPROPERTY(SaveGame)
	TMap<FVector2D, FCellType> ReCreationContainer;

	//Функция, восстанавливающая состояние всех переменных предзагрузки на основе массива ReCreationContainer при загрузке сохранения
	UFUNCTION(BlueprintCallable)
	void ReCreateTerrains(FMapDimensions MapDimensions, FCellCoord& MinNoEmptyTileCoord, FCellCoord& MaxNoEmptyTileCoord);

	//Функция полной очистки матрицы переменных предзагрузки
	UFUNCTION(BlueprintCallable)
	void Clear();
};

