// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoguelikeThing/Widgets/MapEditor/MapEditor.h"
#include "RoguelikeThing/Widgets/MapEditor/CellCoord.h"
#include "RoguelikeThing/Structs/Widgets/MapEditor/MapCell/NeighbourhoodOfCell.h"
#include "MapCell.generated.h"

/*****************************************************************
 * Данный класс является базовым классом для виджета ячейки. Он
 * нужен для правильного взаимодействия блюпринтов и c++ кода.
 *****************************************************************/

//Все возможные типы ячейки
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMapEditor* MyEditor;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCorridorStyle(FNeighbourhoodOfCell NeighbourhoodOfCell);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetRoomStyle();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetEmptinessStyle();

	UFUNCTION(BlueprintImplementableEvent)
	FVector2D getSize();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCellCoord MyCoord;

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
