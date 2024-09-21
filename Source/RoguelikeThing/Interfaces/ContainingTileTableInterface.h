// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RoguelikeThing/Widgets/MapEditor/CellCoord.h"
#include "ContainingTileTableInterface.generated.h"

/**
 * 
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UContainingTileTableInterface : public UInterface
{
	GENERATED_BODY()
};

class ROGUELIKETHING_API IContainingTileTableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateItemAreaContent();

	//Ёвент, вызываемой €чейкой, который определ€ет какой стиль к ней стоит применить
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDBCellValue(FCellCoord CellCoord, UMapCell* CallingCell);
};