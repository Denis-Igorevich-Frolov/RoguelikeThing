// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoguelikeThing/Enumerations/MapEditorBrushType.h"
#include "MapCell.generated.h"

/*****************************************************************
 * ������ ����� �������� ������� ������� ��� ������� ������. ��
 * ����� ��� ����������� �������������� ���������� � c++ ����.
 *****************************************************************/

//��� ��������� ���� ������
UENUM(BlueprintType)
enum class FCellType : uint8 {
	CORRIDOR	UMETA(DisplayName = "Corridor"),
	ROOM		UMETA(DisplayName = "Room"),
	NONE		UMETA(DisplayName = "None")
};

UCLASS()
class ROGUELIKETHING_API UMapCell : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	FVector2D getSize();

	//��������� �� ��� ����� �� ��������� ����. ������ ��� ��������� ������� �� ����� ��� �� ����� �������� ��� ������� �� ���
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMapEditorBrushType* BrushType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCellType CurrentType = FCellType::NONE;

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
