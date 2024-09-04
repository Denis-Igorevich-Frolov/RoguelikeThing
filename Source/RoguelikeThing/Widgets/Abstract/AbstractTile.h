// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoguelikeThing/Widgets/MapEditor/CellCoord.h"
#include <RoguelikeThing/Map/MapMatrix.h>
#include "AbstractTile.generated.h"

/*********************************************************************************************************************
 * Данный класс является базовым для всех тайлов. То есть, если какой-либо виджет заполняет собой таблицу и
 * необходимо, чтобы его оптимизировал TileTablesOptimizationTools, то он должен наследоваться от этого класса.
 *********************************************************************************************************************/
UCLASS()
class ROGUELIKETHING_API UAbstractTile : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	//Координата тайла в пределах своей таблицы
	FCellCoord MyCoord;

public:
	virtual void SetMyCoord(FCellCoord myCoord);
	virtual const FCellCoord GetMyCoord();
	//Виртуальная функция для сброса состояния изменённых дочерних виджетов (если таковые имеются)
	UFUNCTION(BlueprintCallable)
	virtual void ClearFilledCells();
	//Виртуальная функция для удаления всех дочерних виджетов (если таковые имеются)
	virtual void RemoveAllCells();
	//Эвент, вызываемый при добавлении тайла в таблицу
	virtual void OnAddedEvent();
	//Альтернативный эвент, вызываемый при добавлении тайла в таблицу, с классом взаимодействия с БД в качестве аргумента
	virtual void OnAddedEvent(UMapMatrix* MapMatrix);
};
