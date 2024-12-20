// Denis Igorevich Frolov did all this. Once there. All things reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RoguelikeThing/Map/TerrainOfTilesContainer.h"
#include "RoguelikeThing/Widgets/MapEditor/CellCoord.h"
#include "MapSaver.generated.h"

/*******************************************************************
 * Данный класс является классом, создающим файл сохранения .sav
 *******************************************************************/

UCLASS(BlueprintType)
class ROGUELIKETHING_API UMapSaver : public USaveGame
{
	GENERATED_BODY()

private:
	/* Контейнер, хранящий все переменные предзагрузки текущей карты. Чтобы восстанвить состояние
	 * предзагрузки карты посли её загрузки из файла .sav необходимо вызвать функцию ReCreateTerrains */
	UPROPERTY(VisibleAnywhere)
	UTerrainOfTilesContainer* TerrainOfTilesContainer;

	//Бинарный массив для сохранения и загрузки переменной TerrainOfTilesContainer
	UPROPERTY(VisibleAnywhere)
	TArray<uint8> BinTerrainOfTilesContainer;
	
public:

	//Габариты сохранённой карты
	UPROPERTY(VisibleAnywhere)
	FMapDimensions MapDimensions;

	/* Минимальные и максимальные координаты непустых тайлов карты. После вызова функции
	 * ReCreateTerrains переменной TerrainOfTilesContainer данные координаты актуализируются */
	UPROPERTY(VisibleAnywhere)
	FCellCoord MinNoEmptyTileCoord = FCellCoord(-1, -1);
	UPROPERTY(VisibleAnywhere)
	FCellCoord MaxNoEmptyTileCoord = FCellCoord(-1, -1);

	//Хеш файла .db, по которому производится проверка измененился ли извне файл базы данных с момента последнего сохранения
	UPROPERTY(VisibleAnywhere)
	FString MapDataBaseHash;

	//Функция, сериализующая переданную переменную terrainOfTilesContainer
	void SaveTerrainOfTilesContainer(UTerrainOfTilesContainer* terrainOfTilesContainer);
	//Функция, десериализующая переданную переменную terrainOfTilesContainer
	UTerrainOfTilesContainer* LoadTerrainOfTilesContainer();
};
