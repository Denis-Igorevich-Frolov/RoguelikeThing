﻿// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Blueprint/UserWidget.h"
#include <Components/UniformGridSlot.h>
#include "RoguelikeThing/Widgets/Abstract/AbstractTile.h"
#include "CoordWrapperOfTable.generated.h"

/*******************************************************************************************************************************************
 * Данный класс является координатной обёрткой для UniformGridPanel. С помощью этой обёртки можно легко получить элемент по переданным
 * строке и столбцу. Также имеется возможность быстро получить первый (левый нижний) и последний (правый верхний) элемент. Все строки
 * и столбцы могут иметь отрицательный порядковый номер, что позволяет расширять матрицу в любую стророну.
 *******************************************************************************************************************************************/

DECLARE_LOG_CATEGORY_EXTERN(CoordWrapperOfTable, Log, All);

/* Так как в анриале сделать массив массивов просто так нельзя, здесь делается этот класс - болванка,
 * имеющий в себе массив, представляющий из себя строку элементов таблицы. Затем из этих строк будет
 * сделан другой массив, который из себя уже будет представлять матрицу всего контента таблицы */
UCLASS(BlueprintType)
class ROGUELIKETHING_API UWrapperRow : public UObject
{
	GENERATED_BODY()
private:
	//Массив слотов абстрактных тайлов, которые находятся в массиве Row. Эти массивы соотносятся, то есть по одному индексу можно получить и тайл и его слот
	UPROPERTY()
	TMap<int32, UUniformGridSlot*> RowGrid;
	UPROPERTY()
	TMap<int32, UAbstractTile*> Row;
public:
	UAbstractTile* FindWidget(int Key);
	UUniformGridSlot* FindGridSlot(int Key);
	//Функция, удаляющая эллемент по переданному ключу
	bool RemoveWidget(int Key);
	//Функция, исключающая эллемент из координатной обёртки, но не удаляющая его из мира
	bool RemoveIndex(int Key);
	bool AddWidget(int Key, UAbstractTile* Widget, UUniformGridSlot* GridSlot);
	bool HasAnyEllements();
	bool Contains(int Key);
	void Clear();
};

/* Это структура экземпляра координаты. Есть строка, есть столбец - всё просто. Если координата была 
 * создана через пустой конструкторо она непроинициализирована, иначе она полностью готова к работе */
USTRUCT(BlueprintType)
struct FGridCoord
{
	GENERATED_BODY()

private:
	bool isInit = false;

public:
	/* Создание координаты через пустой конструктор делает координату непроинициализированной, почти ничего
	 * не будет работать с такой координатой. Требуются непроинициализированые координаты для функций, которым
	 * в любом случае надо вернуть какую-нибудь координату, а возврат непроинициализированых координат от таких
	 * функций означает ошибку при их выполнении */
	FGridCoord();
	FGridCoord(int row, int col);
	//Проверка проинициализированности координаты
	bool getIsInit();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Col;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Row;

	FString ToString();

	bool operator == (const FGridCoord& Coord) const;
	bool operator != (const FGridCoord& Coord) const;

	FGridCoord operator + (const FGridCoord Bias) const;
	FGridCoord operator - (const FGridCoord Bias) const;
};

//Данная структура отображает габариты в координатах - то есть прямоугольный диапазон ячеек от наименьшей координаты до наибольшей
USTRUCT(BlueprintType)
struct FGridDimensions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridCoord Min;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGridCoord Max;

	FGridDimensions(FGridCoord min = FGridCoord(), FGridCoord max = FGridCoord());

	FString ToString();
	bool IsEmpty();
	//Проверка на то перересекаются ли переданные габариты с текущими. Если да, то возвращается true
	bool DoTheDimensionsIntersect(FGridDimensions OtherDimensions);

	bool operator == (const FGridDimensions& Dimensions) const;
	bool operator != (const FGridDimensions& Dimensions) const;
	FGridDimensions operator + (const FGridDimensions Bias) const;
	FGridDimensions operator - (const FGridDimensions Bias) const;
};

UCLASS(BlueprintType)
class ROGUELIKETHING_API UCoordWrapperOfTable : public UObject
{
	GENERATED_BODY()
private:
	//Менеджер высокого уровня для экземпляра запущенной игры
	UPROPERTY()
	UMyGameInstance* GameInstance;

	UPROPERTY()
	TMap<int32, UWrapperRow*> ColArr;
	int NumberOfItemsInTable = 0;

	FGridCoord MinCoord;
	FGridCoord MaxCoord;

public:
	UCoordWrapperOfTable(const FObjectInitializer& Object);

	UFUNCTION(BlueprintCallable)
	bool AddWidget(int row, int col, UAbstractTile* Widget, UUniformGridSlot* GridSlot);
	UFUNCTION(BlueprintCallable)
	UAbstractTile* FindWidget(int row, int col);
	//Поиск слота таблицы, который соотносится с итемом координатной обёртки
	UFUNCTION(BlueprintCallable)
	UUniformGridSlot* FindGridSlot(int row, int col);
	//Функция, удаляющая эллемент по переданной координате
	UFUNCTION(BlueprintCallable)
	bool RemoveWidget(int row, int col);
	//Функция, исключающая эллемент из координатной обёртки, но не удаляющая его из мира
	UFUNCTION(BlueprintCallable)
	bool RemoveCoord(int row, int col);
	UFUNCTION(BlueprintCallable)
	bool HasAnyEllements();
	UFUNCTION(BlueprintCallable)
	const int getNumberOfItemsInTable();
	UFUNCTION(BlueprintCallable)
	void Clear();

	//Получение самой левой нижней координаты
	UFUNCTION(BlueprintCallable)
	FGridCoord getMinCoord();
	//Получение самой правой верхней координаты
	UFUNCTION(BlueprintCallable)
	FGridCoord getMaxCoord();

	//Проверка на то перересекаются ли переданные габариты. Если да, то возвращается true
	UFUNCTION(BlueprintCallable)
	bool DoTheDimensionsIntersect(FGridDimensions Dimensions_1, FGridDimensions Dimensions_2);

	void setMinCoord(FGridCoord minCoord);
	void setMaxCoord(FGridCoord maxCoord);
};
