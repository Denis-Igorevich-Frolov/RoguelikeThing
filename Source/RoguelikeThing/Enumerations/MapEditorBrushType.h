// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "MapEditorBrushType.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class FMapEditorBrushType : uint8 {
	HORISONTAL_CORRIDOR		UMETA(DisplayName = "Horizontal corridor"),
	VERTICAL_CORRIDOR		UMETA(DisplayName = "Vertical corridor"),
	ROOM					UMETA(DisplayName = "Room"),
	ERASER					UMETA(DisplayName = "Eraser"),
};

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UMapEditorBrushType : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMapEditorBrushType BrushType;
};