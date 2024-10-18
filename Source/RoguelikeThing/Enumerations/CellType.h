// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CellType.generated.h"

/**
 * 
 */
 //Все возможные типы ячейки
UENUM(BlueprintType)
enum class FCellType : uint8 {
	Emptiness		UMETA(DisplayName = "Emptiness"),
	Error		UMETA(DisplayName = "Error"),
	Corridor	UMETA(DisplayName = "Corridor"),
	Room		UMETA(DisplayName = "Room")
};

