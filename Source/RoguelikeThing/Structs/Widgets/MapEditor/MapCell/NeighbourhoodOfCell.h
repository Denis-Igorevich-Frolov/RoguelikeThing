// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "NeighbourhoodOfCell.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FNeighbourhoodOfCell
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool SomethingOnRight = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool SomethingOnLeft = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool SomethingOnTop = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool SomethingOnBottom = false;

	/* Переменная, отражающая то была ли структура проинициализирована или сделана через конструктор по умолчанию.
	 * Непроинициализированные экземпляры считаются невалидными и возвращаются функциями при ошибке */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsInit = false;

	/* Конструктор по умолчанию делает экземпляры, у кторых IsInit равна false. Такие структуры будут считаться
	 * невалидными, их следует заполнить и после этого установить переменной IsInit значение true вручную */
	FNeighbourhoodOfCell();
	FNeighbourhoodOfCell(bool SomethingOnRight, bool SomethingOnLeft, bool SomethingOnTop, bool SomethingOnBottom);
};
