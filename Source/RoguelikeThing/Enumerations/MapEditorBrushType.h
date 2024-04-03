// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "MapEditorBrushType.generated.h"

/**********************************************************************************
 * ������ ����� �������� ������� �������� ���� �����. ��� UMapEditorBrushType
 * �� ����� ������ ���� ����������, �� �� �������� ������� ����������� UObject,
 * ����� ��� ����� ����� ���� ����������� � ������� �� ���������
 **********************************************************************************/

//������������ ���� ��������� ����� �����
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

	//������ �������� ���� �����, ������ ����� ����� ������ � ��� ������ ��������������, ������� ����������� ������

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