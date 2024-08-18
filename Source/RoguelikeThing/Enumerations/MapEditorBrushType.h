// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "MapEditorBrushType.generated.h"

/**********************************************************************************
 * Данный класс является классом текущего типа кисти. Сам UMapEditorBrushType
 * по факту должен быть структурой, но он является классом наследником UObject,
 * чтобы его легко можно было прередавать и хранить по указателю
 **********************************************************************************/

//Перечисление всех возможных типов кисти
UENUM(BlueprintType)
enum class FMapEditorBrushType : uint8 {
	Emptiness	 = 0     UMETA(DisplayName = "Emptiness"),
	Error		 = 1     UMETA(DisplayName = "Error"),
	Corridor	 = 2     UMETA(DisplayName = "Corridor"),
	Room		 = 3     UMETA(DisplayName = "Room"),
	MOVEMAP		 = 4	 UMETA(DisplayName = "Move map"),
};

UCLASS(Blueprintable, BlueprintType)
class ROGUELIKETHING_API UMapEditorBrushType : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMapEditorBrushType BrushType;

	//Помимо текущего типа кисти, данный класс также хранит и все прочие характеристики, которые сопутствуют кистям

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