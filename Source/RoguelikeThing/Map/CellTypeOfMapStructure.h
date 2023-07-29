// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**************************************************************************************
 * Данное перечисление включает в себя все типы клеток во фрагменте структуры карты
 **************************************************************************************/

 /* Зарезервированно под ошибку всегда именно число 1, а не 0 потому что матрица разряженная,
  * и для экономии размера файла он весь забит значениями NULL, что позволяет не хранить в
  * ячейках нули, которые и так возвращаются запросом при попытке считать эти NULL'ы.
  * Соответственно куда лучше для нуля оставить значение "пустота", а для следующего же
  * значения сделать зарезервированность под ошибку.*/
UENUM(BlueprintType)
enum class ECellTypeOfMapStructure : uint8 {
    Emptiness   = 0     UMETA(DisplayName = "Emptiness"),
    Error       = 1     UMETA(DisplayName = "Error"),
    Corridor    = 2     UMETA(DisplayName = "Corridor"),
    RoomEdge    = 3     UMETA(DisplayName = "RoomEdge"),
    RoomCenter  = 4     UMETA(DisplayName = "RoomCenter"),
};
