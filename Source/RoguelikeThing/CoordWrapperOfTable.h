// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Components/Widget.h"
#include "CoordWrapperOfTable.generated.h"

/*******************************************************************************************************************************************
 * Данный класс является координатной обёрткой для UniformGridPanel. С помощью этой обёртки можно легко получить элемент по переданным
 * строке и столбцу. Также имеется возможность быстро получить первый (левый нижний) и последний (правый верхний) элемент. Все строки
 * и столбцы могут иметь отрицательный порядковый номер, что позволяет расширять матрицу в любую стророну.
 *******************************************************************************************************************************************/

/* Так как в анриале сделать массив массивов просто так нельзя, здесь делается этот класс - болванка,
 * имеющий в себе массив, представляющий из себя строку элементов таблицы. Затем из этих строк будет
 * сделан другой массив, который из себя уже будет представлять матрицу всего контента таблицы */
class WrapperRow {
private:
	TMap<int32, UWidget*> Row;
public:
	UWidget* FindWidget(int Key);
	bool AddWidget(int Key, UWidget* Widget);
	bool HasAnyEllements();
	bool Contains(int Key);
};

/* Это структура экземпляра координаты. Есть строка, есть столбец - всё просто. Если координата была 
 * создана через пустой конструкторо она непроинициализирована, иначе она полностью готова к работе */
struct GridCoord
{
private:
	bool isInit = false;

public:
	/* Создание координаты через пустой конструктор делает координату непроинициализированной, почти ничего
	 * не будет работать с такой координатой. Требуются непроинициализированые координаты для функций, которым
	 * в любом случае надо вернуть какую-нибудь координату, а возврат непроинициализированых координат от таких
	 * функций означает ошибку при их выполнении */
	GridCoord();
	GridCoord(int row, int col);
	//Проверка проинициализированности координаты
	bool getIsInit();

	int Col;
	int Row;
};

UCLASS(BlueprintType)
class ROGUELIKETHING_API UCoordWrapperOfTable : public UObject
{
	GENERATED_BODY()
private:
	TMap<int32, WrapperRow*> Col;
	int NumberOfItemsInTable = 0;

	GridCoord MinCoord;
	GridCoord MaxCoord;

public:
	~UCoordWrapperOfTable();

	UFUNCTION(BlueprintCallable)
	bool AddWidget(int row, int col, UWidget* Widget);
	UFUNCTION(BlueprintCallable)
	UWidget* FindWidget(int row, int col);
	UFUNCTION(BlueprintCallable)
	bool HasAnyEllements();
	UFUNCTION(BlueprintCallable)
	const int getNumberOfItemsInTable();
	UFUNCTION(BlueprintCallable)
	void Clear();

	//Получение самой левой нижней координаты
	GridCoord getMinCoord();
	//Получение самой правой верхней координаты
	GridCoord getMaxCoord();
};
