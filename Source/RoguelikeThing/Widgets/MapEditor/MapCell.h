// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RoguelikeThing/Widgets/Abstract/AbstractTile.h"
#include "RoguelikeThing/Widgets/MapEditor/MapEditor.h"
#include "RoguelikeThing/Widgets/MapEditor/CellCoord.h"
#include "RoguelikeThing/Structs/Widgets/MapEditor/MapCell/NeighbourhoodOfCell.h"
#include "MapCell.generated.h"

/*****************************************************
 * ƒанный класс €вл€етс€ базовым дл€ виджета €чейки
 *****************************************************/

DECLARE_LOG_CATEGORY_EXTERN(MapCell, Log, All);

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
	//ћенеджер высокого уровн€ дл€ экземпл€ра запущенной игры
	UPROPERTY()
	UMyGameInstance* GameInstance;

	// оордината родительского тайла, внутри которого расположена эта €чейка
	FCellCoord* CoordOfParentTile;
	//ƒлинна стороны родительского тайла. ѕередаЄтс€ вместе с его координатой
	int MapTileLength = 0;

public:
	UMapCell(const FObjectInitializer& Object);
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

	//‘ункци€, возвращающа€ размер виджета €чейки
	UFUNCTION(BlueprintImplementableEvent)
	FVector2D getSize();

	void SetCoordOfParentTile(UPARAM(ref)FCellCoord& coordOfParentTile, int mapTileLength);

	UFUNCTION(BlueprintCallable)
	FCellCoord GetCoordOfParentTile();
    void SetMyCoord(FCellCoord myCoord) override;

	/* ‘ункци€, возвращающа€ глобальную координату €чейки, то есть с учЄтом координаты родительского тайла. “ак, например,
	 * €чейка 0:1, наход€ща€с€ в тайле 0:3, при длине стороны тайла в 5, будет имень глобальную координату 0:16 */
	UFUNCTION(BlueprintCallable)
	FCellCoord GetMyGlobalCoord();
	//‘ункци€, возвращающа€ координату €чейки внутри своего родительского тайла
	UFUNCTION(BlueprintCallable)
	FCellCoord GetMyLocalCoord();

	//“екущий тип (стиль) €чейки
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
