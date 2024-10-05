// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MapDimensions.generated.h"

/**
 * 
 */
 //Структура габаритов карты
USTRUCT(BlueprintType)
struct FMapDimensions
{
	GENERATED_BODY()

	//Минимальная координата стобца
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinCol;
	//Максимальная координата столбца
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxCol;
	//Минимальная координата строки
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MinRow;
	//Максимальная координата строки
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaxRow;
	/* Число строк и столбцов в матрице фрагмента карты. Это число всегда должно быть
	 * кратно длине тайла, если это не так, то оно усечётся до ближайшего кратного.
	 * Фрагменты карты нужны для хранения большого количества ячеек отдельным кластером в БД */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 TableLength;
	/* Число строк и столбцов тайлов, на которые будет разбита карта. Она разбивается на маленькие
	 * тайлы для оптимизации - отображаться будут только те тайлы, которые сейчас видны */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MapTileLength;

	/* Переменная отражающая работоспособность структуры. Если она равна false,
	 * то она непроинициализирована и сгенерерована через пустой конструктор */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isValid;

	FMapDimensions(int32 MinRow, int32 MaxRow, int32 MinCol, int32 MaxCol,
		int32 TableLength, int32 MapTileLength, bool isValid = true);

	/* Пустой конструктор по умолчанию.
	 * Созданный с помощью него экземпляр не будет считаться валидным.
	 * Создавать такой объект следует, если функция должна вернуть
	 * экземпляр MapDimensions, но её работа была выполнена неправильно */
	FMapDimensions();
};
