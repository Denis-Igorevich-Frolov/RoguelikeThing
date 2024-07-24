// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RoguelikeThing/Widgets/Abstract/AbstractTile.h"
#include "RoguelikeThing/Widgets/MapEditor/MapEditor.h"
#include "RoguelikeThing/Widgets/MapEditor/CellCoord.h"
#include "RoguelikeThing/Structs/Widgets/MapEditor/MapCell/NeighbourhoodOfCell.h"
#include "MapCell.generated.h"

/*****************************************************************
 * ƒанный класс €вл€етс€ базовым классом дл€ виджета €чейки. ќн
 * нужен дл€ правильного взаимодействи€ блюпринтов и c++ кода.
 *****************************************************************/

//¬се возможные типы €чейки
UENUM(BlueprintType)
enum class FCellType : uint8 {
	CORRIDOR	UMETA(DisplayName = "Corridor"),
	ROOM		UMETA(DisplayName = "Room"),
	NONE		UMETA(DisplayName = "None")
};

UCLASS()
class ROGUELIKETHING_API UMapCell : public UAbstractTile
{
	GENERATED_BODY()

private:
	FCellCoord* CoordOfParentTile;
	int MapTileLength = 0;

public:
	//”казатель на редактор карт, который управл€ет этой €чейкой
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMapEditor* MyEditor;

	//‘ункци€, устанавливающа€ €чейке стиль коридора исход€ из еЄ окружени€
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCorridorStyle(FNeighbourhoodOfCell NeighbourhoodOfCell);

	//‘ункци€, устанавливающа€ €чейке стиль комнаты
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetRoomStyle();

	//‘ункци€, устанавливающа€ €чейке пустой стиль
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetEmptinessStyle();

	UFUNCTION(BlueprintImplementableEvent)
	FVector2D getSize();

	void SetCoordOfParentTile(UPARAM(ref)FCellCoord& coordOfParentTile, int mapTileLength);

	UFUNCTION(BlueprintCallable)
	FCellCoord GetCoordOfParentTile();
    void SetMyCoord(FCellCoord myCoord) override;

	UFUNCTION(BlueprintCallable)
	FCellCoord GetMyGlobalCoord();
	UFUNCTION(BlueprintCallable)
	FCellCoord GetMyLocalCoord();

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
