// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "MapEditorBrushType.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class FMapEditorBrushType : uint8 {
	CORRIDOR	UMETA(DisplayName = "Corridor"),
    ROOM		UMETA(DisplayName = "Room"),
	ERASER		UMETA(DisplayName = "Eraser"),
};

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UMapEditorBrushType : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMapEditorBrushType BrushType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool Researched = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HaveRightWall = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HaveLeftWall = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HaveRightDoor = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool HaveLeftDoor = false;
};