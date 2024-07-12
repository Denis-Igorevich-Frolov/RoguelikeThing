// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoguelikeThing/Widgets/MapEditor/CellCoord.h"
#include "AbstractTile.generated.h"

/**
 * 
 */
UCLASS()
class ROGUELIKETHING_API UAbstractTile : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	FCellCoord MyCoord;

public:
	virtual void SetMyCoord(FCellCoord myCoord);
};
