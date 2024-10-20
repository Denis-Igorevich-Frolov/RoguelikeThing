// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RoguelikeThing/Widgets/MapEditor/CellCoord.h"
#include "ContainingTileTableInterface.generated.h"

/*************************************************************************************************
 * Данный интерфейс должны имплементировать все классы, которые хранят в себе тайловую таблицу
 *************************************************************************************************/

UINTERFACE(MinimalAPI, Blueprintable)
class UContainingTileTableInterface : public UInterface
{
	GENERATED_BODY()
};

class ROGUELIKETHING_API IContainingTileTableInterface
{
	GENERATED_BODY()

public:
	/* Ивент, говорящий о том, что отображение таблицы следует обновить. Например
	 * при загрузке новой таблицы или при изменении размеров текущей */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateItemAreaContent();

	//Эвент, вызываемый ячейкой, когда на неё нажали
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnCellPressed(FCellCoord CellCoord, UMapCell* CallingCell);
};