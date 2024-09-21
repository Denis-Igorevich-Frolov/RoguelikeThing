// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RoguelikeThing/Widgets/Abstract/AbstractTile.h"
#include "RoguelikeThing/Widgets/MapEditor/MapEditor.h"
#include "RoguelikeThing/Widgets/MapEditor/CellCoord.h"
#include "RoguelikeThing/Structs/Widgets/MapEditor/MapCell/NeighbourhoodOfCell.h"
#include "MapCell.generated.h"

/*****************************************************
 * Данный класс является базовым для виджета ячейки
 *****************************************************/

DECLARE_LOG_CATEGORY_EXTERN(MapCell, Log, All);

//Все возможные типы ячейки
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
	//Менеджер высокого уровня для экземпляра запущенной игры
	UPROPERTY()
	UMyGameInstance* GameInstance;

	//Координата родительского тайла, внутри которого расположена эта ячейка
	FCellCoord* CoordOfParentTile;
	//Длинна стороны родительского тайла. Передаётся вместе с его координатой
	int MapTileLength = 0;

public:
	UMapCell(const FObjectInitializer& Object);
	//Указатель на редактор карт, который управляет этой ячейкой
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TScriptInterface<IContainingTileTableInterface> MyContainer;

	//Функция, устанавливающая ячейке стиль коридора исходя из её окружения
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetCorridorStyle(FNeighbourhoodOfCell NeighbourhoodOfCell);

	//Функция, устанавливающая ячейке стиль комнаты
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetRoomStyle();

	//Функция, устанавливающая ячейке пустой стиль
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetEmptinessStyle();

	//Функция, возвращающая размер виджета ячейки
	UFUNCTION(BlueprintImplementableEvent)
	FVector2D getSize();

	void SetCoordOfParentTile(UPARAM(ref)FCellCoord& coordOfParentTile, int mapTileLength);

	UFUNCTION(BlueprintCallable)
	FCellCoord GetCoordOfParentTile();
    void SetMyCoord(FCellCoord myCoord) override;
	const FCellCoord GetMyCoord() override;

	/* Функция, возвращающая глобальную координату ячейки, то есть с учётом координаты родительского тайла. Так, например,
	 * ячейка 0:1, находящаяся в тайле 0:3, при длине стороны тайла в 5, будет имень глобальную координату 0:16 */
	UFUNCTION(BlueprintCallable)
	FCellCoord GetMyGlobalCoord();
	//Функция, возвращающая координату ячейки внутри своего родительского тайла
	UFUNCTION(BlueprintCallable)
	FCellCoord GetMyLocalCoord();

	//Текущий тип (стиль) ячейки
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
